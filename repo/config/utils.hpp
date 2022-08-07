#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <string>
# include <sstream>
# include <iostream>
# include <vector>
# include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

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
std::string URLencode(std::string url, std::string encodeSet, bool encodeNonPrintables);
std::string URLgetFileName(std::string url);
std::string URLdecode(std::string url);
std::string URLremoveQueryParams(std::string url);
std::string URLgetQueryParams(std::string url);
bool isValidURLPath(std::string url);
std::string getFileContents(std::string path);
int nOccurrence(std::string heap, std::string needle);
std::vector<std::string> configSplit(const std::string &s, char seperator);

# define FILE_LINE getFileAndLine(__FILE__, __LINE__)

#endif
