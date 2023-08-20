#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>
#include "File.h"

class TextEditor {
public:
	TextEditor();

	void run();
	void openFile(std::string fileName);

	std::string fileName;
private:
	void display(); // display the text on the screen
	void clearScreen(); 
	void insertCharacter(int ch); 
	void deleteCharacter();
	int calcOutOfScreenLines(); //calculates number of lines which are larger than screen coordination and are being displayed in multiple lines 
								//(how many extra Screen lines is used in order to display the text correctly)
	void insertNewline();
	void handleKeyEvent(KEY_EVENT_RECORD& keyEvent); // handles keyboaed inputs
	void handleMouseEvent(MOUSE_EVENT_RECORD& MouseEvent); // handles mouse inputs

	void save();

	std::vector<std::string> lines; // it stores the text, each line is a string 
	int currRow, currCol;
	HANDLE hConsole, hInput; // handle to output and input
	COORD screenSize; 
	COORD cursor; // cursor position
	COORD cursorOffset; // it is used to adjust cursor position in different situations
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int countOfOutOfScreenLines;	// number of lines which are larger than screen coordination and are being displayed in multiple lines 
									//(how many extra Screen lines is used in order to display the text correctly)
									// it is calculated in display() function
	bool ctrlState = false; // state of the control key (true if ctrl key is down and false when it's up)
	bool saved = false; // tracks file saving status (true if the text is saved in the file and false if it is not saved or any changes happend after saving)
};
