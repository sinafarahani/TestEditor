#include "TextEditor.h"

TextEditor::TextEditor() :
	currRow(0),
	currCol(0),
	cursor({ 0, 0 }),
	cursorOffset({ 0, 0 }),
	countOfOutOfScreenLines(0)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	screenSize = { short(csbi.srWindow.Right - csbi.srWindow.Left + 1), short(csbi.srWindow.Bottom - csbi.srWindow.Top + 1) };
	lines.push_back("");
}

void TextEditor::run()
{
	while (true) {

		DWORD numEventsRead, numEvents;

		GetNumberOfConsoleInputEvents(hInput, &numEvents);

		INPUT_RECORD* inputRecord = new INPUT_RECORD[numEvents];;

		ReadConsoleInput(hInput, inputRecord, numEvents, &numEventsRead);

		for (DWORD i = 0; i < numEventsRead; ++i) {
			if (inputRecord[i].EventType == KEY_EVENT) {
				handleKeyEvent(inputRecord[i].Event.KeyEvent);
			}
			if (inputRecord[i].EventType == MOUSE_EVENT) {
				handleMouseEvent(inputRecord[i].Event.MouseEvent);
			}
			if (inputRecord[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
				COORD newScreenSize;
				newScreenSize = inputRecord[i].Event.WindowBufferSizeEvent.dwSize;
				if (newScreenSize.X != screenSize.X) {
					screenSize = newScreenSize;
					cursorOffset = { 0,0 };
					currRow = lines.size() - 1;
					currCol = lines[currRow].length();
				}
			}
			display();
		}
		delete[] inputRecord;
	}
}

void TextEditor::openFile(std::string fileName)
{
	File file(fileName);
	lines = file.ReadFrom();
	this->fileName = fileName;
	saved = true;
}

void TextEditor::display()
{
	clearScreen();
	countOfOutOfScreenLines = 0;
	for (const auto& line : lines) {
		std::cout << line << '\n';

		if (line.length() > screenSize.X) {
			countOfOutOfScreenLines += line.length() / screenSize.X;
		}
	}
	cursor = { short((currCol % screenSize.X) + cursorOffset.X),short(currRow + countOfOutOfScreenLines + cursorOffset.Y) };
	SetConsoleCursorPosition(hConsole, cursor);
}

void TextEditor::clearScreen()
{
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	DWORD dwConSize;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
}

void TextEditor::insertCharacter(int ch)
{
	lines[currRow].insert(currCol, 1, ch);
	currCol++;
	if (cursor.X == screenSize.X - 1 && currCol != lines[currRow].length())
		cursorOffset.Y++;
	if (countOfOutOfScreenLines != calcOutOfScreenLines() && currCol != lines[currRow].length())
		cursorOffset.Y--;
	saved = false;
}

void TextEditor::deleteCharacter()
{
	if (currCol > 0) {
		lines[currRow].erase(currCol - 1, 1);
		currCol--;
		if (cursor.X == 0 && currCol != lines[currRow].length())
			cursorOffset.Y--;
		if (countOfOutOfScreenLines != calcOutOfScreenLines() && currCol != lines[currRow].length())
			cursorOffset.Y++;
	}
	else if (currRow > 0) {
		int oldNumberOfLines = lines.size() + calcOutOfScreenLines();
		currCol = lines[currRow - 1].length();
		lines[currRow - 1] += lines[currRow];
		lines.erase(lines.begin() + currRow);
		currRow--;
		cursorOffset.Y -= (lines.size() + calcOutOfScreenLines()) - oldNumberOfLines + 1;
	}
	saved = false;
}

int TextEditor::calcOutOfScreenLines()
{
	int count = 0;
	for (const auto& line : lines) {
		if (line.length() > screenSize.X) {
			count += line.length() / screenSize.X;
		}
	}
	return count;
}

void TextEditor::insertNewline()
{
	int oldNumberOfLines = lines.size() + calcOutOfScreenLines();
	std::string restOfLine = lines[currRow].substr(currCol);
	lines[currRow].erase(currCol);
	lines.insert(lines.begin() + currRow + 1, restOfLine);
	currRow++;
	currCol = 0;
	cursorOffset.Y += oldNumberOfLines - (lines.size() + calcOutOfScreenLines()) + 1;
	saved = false;
}

void TextEditor::handleKeyEvent(KEY_EVENT_RECORD& keyEvent)
{
	if (keyEvent.bKeyDown) {
		if (ctrlState) {
			switch (keyEvent.wVirtualKeyCode) {
			case 'S':
				save();
				break;
			case 'Q':
				clearScreen();
				if (saved)
				{
					exit(0);
				}
				else {
					std::cout << "do you want to save changes? (Y/N) Esc to cancel" << std::endl;
				CH:
					auto ch = _getch();
					if (ch == 'y') {
						save();
						exit(0);
					}
					else if (ch == 'n') {
						exit(0);
					}
					else if (ch == VK_ESCAPE) {
						ctrlState = false;
						break;
					}
					else
						goto CH;
				}
				break;
			}
		}
		else {
			switch (keyEvent.wVirtualKeyCode) {
			case VK_RETURN:
				insertNewline();
				break;
			case VK_BACK:
				deleteCharacter();
				break;
			case VK_LEFT:
				if (currCol > 0) {

					if (cursor.X == 0)
						cursorOffset.Y--;

					currCol--;
				}
				else if (currRow > 0) {
					currRow--;
					currCol = lines[currRow].length();
				}
				break;
			case VK_RIGHT:
				if (currCol < lines[currRow].length()) {

					if (cursor.X == screenSize.X - 1)
						cursorOffset.Y++;

					currCol++;
				}
				else if (lines.size() - 1 > currRow) {
					currRow++;
					currCol = 0;
				}
				break;
			case VK_UP:
				if (currCol >= screenSize.X) {
					currCol -= screenSize.X;
					cursorOffset.Y--;
				}
				else {
					if (currRow > 0) {
						currRow--;

						if (currCol > lines[currRow].length())
							currCol = lines[currRow].length();

						if (lines[currRow].length() % screenSize.X > currCol)
							currCol += (lines[currRow].length() / screenSize.X) * screenSize.X;
						else
							currCol = lines[currRow].length();
					}
				}
				break;
			case VK_DOWN:
				if ((currCol / screenSize.X + 1) * screenSize.X < lines[currRow].length()) {
					currCol += screenSize.X;

					if (currCol > lines[currRow].length())
						currCol = lines[currRow].length();

					cursorOffset.Y++;
				}
				else
				{
					if (currRow < lines.size() - 1) { //if we are not in the last line
						currRow++;
						if (currCol > lines[currRow].length())
							currCol = lines[currRow].length();

						while (screenSize.X <= currCol)
							currCol -= screenSize.X;
					}
					else { //push the cursor to the end of the text if we are in the last line
						currCol = lines[currRow].length();
					}
				}
				break;
			case VK_TAB:

				break;
			case VK_CONTROL:
				ctrlState = true;
				break;
			default:
				if (keyEvent.uChar.UnicodeChar >= 32 && keyEvent.uChar.UnicodeChar <= 126) {
					insertCharacter(keyEvent.uChar.UnicodeChar);
				}
				break;
			}
		}
	}
	else {
		switch (keyEvent.wVirtualKeyCode) {
		case VK_CONTROL:
			ctrlState = false;
			break;
		}
	}
}

void TextEditor::handleMouseEvent(MOUSE_EVENT_RECORD& MouseEvent)
{
}

void TextEditor::save()
{
	File file(fileName);
	if (file.SaveTo(lines)) {
		saved = true;
	}
	else
		std::cerr << "failed to save the file";
}
