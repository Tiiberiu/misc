// CopyFilesToPath.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>


#define fileList_path ""
#define directory_toSend ""

int checkFileOrDirectory(std::string path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			//it's a directory
			return 1;
		}
		else if (s.st_mode & S_IFREG)
		{
			//it's a file
			return 2;
		}
		else
		{
			//something else
			return 0;
		}
	}
	else
	{
		//error
		return -1;
	}
}

int main(int args , char *argv[])
{

	if (args < 3) {

		return -1;
	}
	std::string text_file_with_filedirs(argv[1]);
	if (checkFileOrDirectory(text_file_with_filedirs) != 1)
		return -1;

	std::string destination_directory_path(argv[2]);
	if (checkFileOrDirectory(destination_directory_path) != 1)
		return -1;

	std::ifstream dirs(text_file_with_filedirs);
	std::string file_path;

	while (std::getline(dirs, file_path)) {

		if (file_path.empty())
			continue;
		if (checkFileOrDirectory(file_path) == 2) {

			int pos = file_path.rfind('\\');
			std::string output_file_name = file_path.substr(pos, file_path.size());

			std::ifstream  src(file_path, std::ios::binary);
			std::ofstream  dst(destination_directory_path + output_file_name, std::ios::binary);

			dst << src.rdbuf();

			src.close();
			dst.close();
		}
		else std::cout << file_path.c_str() << "is not valid file path \n";
	}

    return 0;
}

