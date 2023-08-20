#include "File.h"

File::File(std::string filename)
	:
	fileName(filename)
{}

bool File::checkExistance()
{
	std::fstream file(fileName);
	return file.is_open();
}

bool File::SaveTo(const std::vector<std::string>& lines)
{
	std::ofstream file(fileName);
	if (file.is_open()) {
		for (const auto& line : lines) {
			file << line << '\n';
		}
		file.close();
		return true;
	}
	else
		return false;
}

bool File::Delete()
{
	return (remove(fileName.c_str()) == 0);
}

std::vector<std::string> File::ReadFrom()
{
	std::vector<std::string> lines;
	std::ifstream file(fileName);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
		return lines;
	}
	return std::vector<std::string>();
}
