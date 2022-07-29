#include <iostream>
#include <string>



int  main(int argc, const char** argv) {
	std::string str = "/home/chegashi/fi/f2.txt";
	std::string *str_array = split_url("/");
	while (*str_array != "")
	{
		std::cout << *str_array << std::endl;
		str_array++;
	}
	return 0;
}