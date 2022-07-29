#include <map>
#include <string>
#include "../request/Request.hpp"
#include "../../config/print.hpp"
#include "../../config/error.hpp"
#include "../../config/utils.hpp"

typedef std::map<std::string, std::string> HeaderMap;

#define AUTHORIZATION_HEADER "authorization"

char *ft_strchr(char *s, char c) {
	while (*s) {
		if (*s == c) {
			return s;
		}
		s++;
	}
	return NULL;
}

std::string getWord(std::string phrase, bool (*yes)(char)) {
	std::string ret = "";
	for (std::string::iterator it = phrase.begin(); it != phrase.end(); it++) {
		if (!yes(*it)) {
			break;
		}
		ret += *it;
	}
	return ret;
}

#define IS_SEPARATOR(x) (ft_strchr("()<>@,;:\\\"/[]?={} \t", x))

#define IS_CHAR(x) (std::isalnum(x) \
					|| IS_SEPARATOR(x) \
					|| ft_strchr("!#$%&'*+-.`^_{|}~", x) \
					|| std::iscntrl(x))

bool isValidTokenChar(char c) {
	return (!std::iscntrl(c) && !IS_SEPARATOR(c) && IS_CHAR(c));
}

std::string formulateResponseFromCGI(const Request &req, std::string cgiPath) {
	HeaderMap env;
	const HeaderMap &headers = req.getHeaders();
	HeaderMap::const_iterator authorizationHeader = headers.find(AUTHORIZATION_HEADER);
	if (authorizationHeader != headers.end()) {
		std::string authScheme = getWord(authorizationHeader->second, isValidTokenChar);
		env[AUTHORIZATION_HEADER] = authScheme;
	}
	
}