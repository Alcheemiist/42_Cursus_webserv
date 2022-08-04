#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>


int main(int argc, char const *argv[])
{
	std::ofstream _file;
	_file.open ("iindex.html", std::ios::out);
	// std::ofstream file("/tmp/wwwjs/index.html");
	_file << "<!DOCTYPE html><html><body>";
	_file << std::endl;
	_file.close();
	system("cat iindex.html");
	return 0;
}
