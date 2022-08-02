#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include <string>
#include <vector>
#include  <stdio.h>
#include  <stdlib.h>

#include "../elements.hpp"

errno_t _access_s(
   const char *path,
   int mode
);
errno_t _waccess_s(
   const wchar_t *path,
   int mode
);

std::string *split_url(std::string str);

bool url_is_formated(std::string url);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool url_redirected(std::string url, Server server);

std::vector<std::string>	get_location(int port);

bool	method_is_allowed(std::string method, std::string url, Server server);

bool file_exist(std::string path);

std::string get_location_url(std::string url, Server server);

std::string get_redirection_url(std::string url, Server server);

bool	requested_file_in_root(std::string url, Server server);

bool is_file(std::string url);

bool Location_support_upload(std::string url, Server server);

char    *readAllFile(char *path);

size_t  _getFileSize(const char *fileName);

std::vector<char> read_by_vector(char *path, Response *response);

#endif // !Re