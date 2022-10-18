#include "filechooser.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

void FileChooser::get_file_from_user(std::ifstream &stream)
{
	std::vector<std::string> files = get_files();

	std::cout << "Available files:\n";
	for (auto& file : files) {
		std::cout << '\t'  << file << '\n';
	}
	std::cout << "Choose file to open: ";

	std::string current = "";
	while (true) {
		std::cin >> current;
		if (std::find(files.begin(), files.end(), current) != files.end()) break;
		std::cout << "Couldn't open file with name: " << current << "\nPlease try again.\n";
	}
	stream.open(current);
}

std::vector<std::string> FileChooser::get_files()
{
	std::vector<std::string> files;
	for (auto& file : std::filesystem::directory_iterator(".")) {
		if (file.path().extension() == ".ch8") {
			files.push_back(file.path().filename().string());
		}
	}
	return files;
}