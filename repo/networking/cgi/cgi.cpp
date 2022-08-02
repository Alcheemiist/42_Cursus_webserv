#include <map>
#include <string>
#include "../request/Request.hpp"
#include "../../config/print.hpp"
#include "../../config/error.hpp"
#include "../../config/utils.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef std::map<std::string, std::string> HeaderMap;
typedef std::string str;

#define AUTHORIZATION_HEADER "authorization"
#define AUTHORIZATION_ENV "AUTH_TYPE"
#define CONTENT_LENGTH_HEADER "content-length"
#define CONTENT_LENGTH_ENV "CONTENT_LENGTH"
#define CONTENT_TYPE_HEADER "content-type"
#define CONTENT_TYPE_ENV "CONTENT_TYPE"
#define GATEWAY_INTERFACE_ENV "GATEWAY_INTERFACE"
#define PATH_INFO_ENV "PATH_INFO"
#define SCHEME "http"
#define PATH_TRANSLATED_ENV "PATH_TRANSLATED"
#define HOST_HEADER "host"
#define QUERY_STRING_ENV "QUERY_STRING"
#define REMOTE_ADDR_ENV "REMOTE_ADDR"
#define REMOTE_HOST_ENV "REMOTE_HOST"
#define REMOTE_IDENT_ENV "REMOTE_IDENT"
#define REMOTE_USER_ENV "REMOTE_USER"

const char *ft_strchr(const char *s, char c) {
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

#define IS_SEPARATOR(x) (!!ft_strchr("()<>@,;:\\\"/[]?={} \t", x))

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
	(void)cgiPath;
	HeaderMap env;
	const HeaderMap &headers = req.getHeaders();

	// AUTHORIZATION
	// env[AUTHORIZATION_ENV] = "";
	// CONTENT_LENGTH
	ADD_ENV_HEADER(CONTENT_LENGTH);
	// CONTENT_TYPE
	ADD_ENV_HEADER(CONTENT_TYPE);
	// GATEWAY_INTERFACE
	env[GATEWAY_INTERFACE_ENV] = "CGI/1.1";
	// PATH_INFO
	env[PATH_INFO_ENV] = URLremoveQueryParams(req.getPath());
	// PATH_TRANSLATED
	ADD_ENV_HEADER_CUSTOM(HOST, {
		env[PATH_TRANSLATED_ENV] = str(SCHEME) + "://" + hit->second + env[PATH_INFO_ENV];
	});
	// QUERY_STRING
	env[QUERY_STRING_ENV] = URLgetQueryParams(req.getPath());
	// REMOTE_ADDR
	const struct sockaddr_in* adr = (const struct sockaddr_in*)&req.getRefClientAddr();
	char remoteAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &adr->sin_addr, remoteAddr, INET_ADDRSTRLEN);
	env[REMOTE_ADDR_ENV] = str(remoteAddr);
	// REMOTE_HOST
	env[REMOTE_HOST_ENV] = str(remoteAddr);
	// REMOTE_IDENT
	env[REMOTE_IDENT_ENV] = "";
	// REMOTE_USER 
	/* not required unless authentication is also required */
	// ADD_ENV_HEADER_CUSTOM(AUTHORIZATION, {
	// });

	throw "remove this throw statement when cgi is done";
	return "";
}