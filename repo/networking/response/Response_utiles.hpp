#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include <string>
#include <vector>
#include "../elements.hpp"

std::string *split_url(std::string str);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool url_redirected(std::string url, Server server);

std::vector<std::string>	get_location(int port);

bool	method_is_allowed(std::string method, std::string url, Server server);

bool file_exist(std::string path);

#endif // !Re
