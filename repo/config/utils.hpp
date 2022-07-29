#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <string>

# include <sstream>

#define PRINT_VALUE(x) println(#x, " = ", x)
#define PRINT_LINE_VALUE(x) println(__FILE__, ":", __LINE__, " ", #x, " = ", x)

template <typename T>
std::string to_string(T val)
{
	std::stringstream stream;
	stream << val;
	return stream.str();
}

int to_int(std::string _s);
std::string getFileAndLine(const char *f, int l);
std::string &to_upper(std::string &in);
std::string &URLencode(std::string &url);
std::string &URLdecode(std::string &url);

# define FILE_LINE getFileAndLine(__FILE__, __LINE__)

#endif
