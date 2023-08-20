#include "TextEditor.h"

int main() {
start:
	std::string fileName;
	std::cout << "Enter the file name: " << std::endl;
	std::getline(std::cin, fileName);
	File file(fileName);
	TextEditor editor;
	if (file.checkExistance()) {
		std::cout << "this file is already exist. do you want to open it? (Y/N)" << std::endl;
	CH:
		auto ch = _getch();
		if (ch == 'y')
			editor.openFile(fileName);
		else if (ch == 'n')
			goto start;
		else
			goto CH;
	}
	else {
		editor.fileName = fileName;
	}
	editor.run();
}
