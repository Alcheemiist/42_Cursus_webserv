#include <map>
#include <string>
#include "../request/Request.hpp"
#include "../../config/print.hpp"
#include "../../config/error.hpp"
#include "../../config/utils.hpp"

typedef std::map<std::string, std::string> HeaderMap;

#define AUTHORIZATION_HEADER "authorization"
#define AUTHORIZATION_ENV "AUTH_TYPE"
#define CONTENT_LENGTH_HEADER "content-length"
#define CONTENT_LENGTH_ENV "CONTENT_LENGTH"
#define CONTENT_TYPE_HEADER "content-type"
#define CONTENT_TYPE_ENV "CONTENT_TYPE"
#define GATEWAY_INTERFACE_ENV "GATEWAY_INTERFACE"

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

#define ADD_ENV_HEADER_CUSTOM(header, exec_block) \
{ HeaderMap::const_iterator hit = headers.find(header ## _HEADER);\
if (hit != headers.end()) exec_block }

#define ADD_ENV_HEADER(header) \
{ HeaderMap::const_iterator hit = headers.find(header ## _HEADER);\
if (hit != headers.end()) { env[header ## _ENV] = hit->second; } }



std::string formulateResponseFromCGI(const Request &req, std::string cgiPath) {
	HeaderMap env;
	const HeaderMap &headers = req.getHeaders();

	ADD_ENV_HEADER_CUSTOM(AUTHORIZATION, {
		std::string authScheme = getWord(hit->second, isValidTokenChar);
		env[AUTHORIZATION_ENV] = authScheme;
	});
	ADD_ENV_HEADER(CONTENT_LENGTH);
	ADD_ENV_HEADER(CONTENT_TYPE);
	env[GATEWAY_INTERFACE_ENV] = "CGI/1.1";
	std::string path = req.getPath();
	// env[PATH_INFO_ENV] = 
}