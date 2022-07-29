#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include <string>
#include <vector>
#include "../elements.hpp"

std::string *split_url(std::string str);
std::string url_matched(std::string url, int port, const std::vector<Server> locations);
std::string url_redirected(std::string url, int port, const std::vector<Server> redirections);
std::vector<std::string>	get_location(int port);
bool	method_is_allowed(std::string url, std::string method, int port, const std::vector<Server> servres);

#endif // !Re
