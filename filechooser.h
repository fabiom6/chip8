#pragma once
#include <vector>
#include <istream>

class FileChooser {
public:
	void get_file_from_user(std::ifstream &stream);
private:
	std::vector<std::string> get_files();
};