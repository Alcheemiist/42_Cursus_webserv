#include <string>
#include <sstream>
#include <iomanip>
#include "utils.hpp"
#include "throwed.hpp"

int to_int(std::string _s) {
	const char *s = _s.c_str();
	if (s == NULL || *s == '\0')
		throw std::invalid_argument("null or empty string argument");

	bool negate = (s[0] == '-');
	if (*s == '+' || *s == '-')
		++s;

	if (*s == '\0')
		throw std::invalid_argument("sign character only.");

	int result = 0;
	while (*s) {
		if (*s < '0' || *s > '9')
			throw std::invalid_argument("invalid input string " + _s);
		result = result * 10 - (*s - '0'); // assume negative number
		++s;
	}
	return negate ? result : -result; //-result is positive!
}

std::string getFileAndLine(const char *f, int l) {
	return std::string(f) + ":" +  to_string(l);
}


std::string &to_upper(std::string &in) {
	for (std::string::iterator it = in.begin(); it != in.end(); it++) {
		*it = std::toupper(*it);
	}
	return in;
}

template <typename T>
std::string int_to_hex(T i) {
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2)
		   << std::hex << (int)i;
	return stream.str();
}

std::string URLencode(std::string url, std::string encodeSet = ";/?:@&=+$, ", bool encodeNonPrintables = true) {
	std::string ret = "";
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (std::find(encodeSet.begin(), encodeSet.end(), *it) != encodeSet.end() || (encodeNonPrintables && !std::isprint(*it))) {
			ret += "%" + int_to_hex(((char)(*it)));
		}
		else {
			ret += *it;
		}
	}
	return ret;
}

std::string URLgetFileName(std::string url) {
	url = URLremoveQueryParams(url);
	url = URLdecode(url);
	return url.substr(url.find_last_of('/'), url.length());
}

#define IS_HEX(x) (std::isdigit(x) || (std::tolower(x) >= 'a' && std::tolower(x) <= 'f'))

std::string URLdecode(std::string url) {
	std::string ret = "";
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (*it == '%') {
			it++;
			if (it == url.end())
				throw std::invalid_argument("unexpected end of string");
			char c1 = *it;
			it++;
			if (it == url.end())
				throw std::invalid_argument("unexpected end of string");
			char c2 = *it;
			if (!(IS_HEX(c1) && IS_HEX(c2)))
				throw std::invalid_argument("found invalid escape sequence");
			std::string hexString = "";
			hexString += c1;
			hexString += c2;
			unsigned int c;   
			std::stringstream ss;
			ss << std::hex << hexString;
			ss >> c;
			ret += c;
		}
		else {
			ret += *it;
		}
	}
	return ret;
}

std::string URLremoveQueryParams(std::string url) {
	if (url.find("?") != (size_t)-1) {
		return url.substr(0, url.find("?"));
	}
	return url;
}

std::string URLgetQueryParams(std::string url) {
	if (url.find("?") != (size_t)-1) {
		return url.substr(url.find("?") + 1, url.length());
	}
	return "";
}

bool isValidURLPath(std::string url) {
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (std::iscntrl(*it)) {
			return false;
		}
	}
	return !throwed<std::string>(URLdecode, url);
}
