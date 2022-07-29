#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include <string>
#include <vector>
#include "../elements.hpp"

std::string *split_url(std::string str);
std::string url_matched(std::string url, int port, const std::vector<server> locations);
std::vector<std::string>	get_location(int port);

#endif // !Re
