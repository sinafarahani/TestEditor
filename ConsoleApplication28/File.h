#pragma once
#include <string>
#include <vector>
#include <fstream>

class File {
public:
	File(std::string filename);
	bool checkExistance(); //check if any file with fileName is already exist
	bool SaveTo(const std::vector<std::string>& lines); // save lines into the file
	bool Delete();
	std::vector<std::string> ReadFrom(); // read the data from file and return them as a vector of strings. each string is one line of the text
private:
	std::string fileName;
};