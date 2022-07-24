#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <string>

# include <sstream>

template <typename T>
std::string to_string(T val)
{
	std::stringstream stream;
	stream << val;
	return stream.str();
}

int to_int(std::string _s);
std::string getFileAndLine(const char *f, int l);

# define FILE_LINE getFileAndLine(__FILE__, __LINE__)

#endif
