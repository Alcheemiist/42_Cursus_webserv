#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include <dirent.h>
#include <sys/types.h>
#include "../elements.hpp"

std::string *split_url(std::string str);

bool url_is_formated(std::string url);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool url_redirected(std::string url, Server server);

std::vector<std::string>	get_location(int port);

bool	method_is_allowed(std::string method, std::string url, Server server);

bool file_exist(std::string path);

std::string get_location_url(std::string url, Server server);

// std::string get_redirection_url(std::string url, Server server);

bool	requested_file_in_root(std::string url);

bool is_file(std::string url);

bool Location_support_upload(std::string url, Server server);

char    *readAllFile(char *path);

size_t  _getFileSize(const char *fileName);

std::vector<char> read_by_vector(char *path, Response *response);
int str_matched(std::string str1, std::string str2);
std::string remove_duplicate_slash(std::string str);
std::string generate_auto_index(std::string url);
#endif