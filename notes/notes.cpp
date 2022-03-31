#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/filebox.hpp>

#include <fstream>
#include <iostream>

int main()
{
	using namespace nana;

	form fm(API::make_center(600, 300));

	nana::textbox editor(fm, "");

	editor.enable_dropfiles(true);
	editor.events().mouse_dropfiles([&editor](const arg_dropfiles& arg) {
		editor.load(arg.files[0]);
		}
	);

	nana::listbox lbox(fm);
	auto header = lbox.append_header("notes");
	auto notes = lbox.at(header);

	{
		std::ifstream file("data.txt");
		uint64_t size;
		while (file.read((char*)&size, sizeof size)) {
			std::string str;
			str.resize(size);
			file.read(str.data(), size);
			notes.append(str);
		}
	}

	lbox.events().selected([&editor, notes, header](const arg_listbox& arg) {
		if (arg.item.selected()) {
			editor.reset(arg.item.text(header));
		}
		else {
			arg.item.text(header, editor.text());
			editor.reset("");
		}
		}
	);


	button add_button{ fm, "Add" };
	add_button.register_shortkey(L'a');
	add_button.events().click(
		[&lbox, &notes, &editor] {
			notes.append(editor.text());
		}
	);

	button open_button{ fm, "Open" };
	open_button.register_shortkey(L'O');
	open_button.events().click(
		[&lbox, &notes, &editor, &fm] {
			filebox fbox(fm, true);
			fbox.allow_multi_select(false);
			const auto result = fbox();
			if (not result.empty())
				editor.load(result[0]);
		}
	);


	button delete_button{ fm, "Delete" };
	delete_button.register_shortkey(L'd');
	delete_button.events().click(
		[&lbox] {
			lbox.erase(lbox.selected());
		}
	);

	button save_button{ fm, "Save" };
	save_button.register_shortkey(L's');
	save_button.events().click(
		[&lbox] {
			std::ofstream file("data.txt");
			for (const auto& item : lbox.at(0)) {
				const auto str = item.text(0);
				const uint64_t size = str.size();
				file.write((const char*)&size, sizeof size);
				file.write(str.data(), size);
			}
		}
	);

	fm.div(
		"vert"
		"< weight = 10% <add_button> <delete_button> <open_button> <save_button> >"
		"< <listbox> <text_editor> >"
	);
	fm["listbox"] << lbox;
	fm["text_editor"] << editor;
	fm["add_button"] << add_button;
	fm["delete_button"] << delete_button;
	fm["save_button"] << save_button;
	fm["open_button"] << open_button;
	fm.collocate();

	fm.show();
	exec();
}