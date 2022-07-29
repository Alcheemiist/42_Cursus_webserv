#include "Response_utiles.hpp"

bool file_exist(std::string path)
{
	struct stat st;
	return (stat(path.c_str(), &st) == 0);
}

std::string *split_url(std::string str)
{
	std::string *str_array = new std::string[str.size() + 1];
	int i = 1;
	str_array[0] = "/";
	str = str.substr(1);
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
		if (*it == '/')
			i++;
		else
			str_array[i] += *it;
	str_array[i] = "";
	return str_array;
}

std::string url_matched(std::string url, int port, const std::vector<server> locations)
{
	server serv;
	std::string path;
	std::vector<location> loc= serv.get_location();

	for (std::vector<server>::const_iterator it_server = locations.begin();
			it_server != locations.end() || it_server->get_listen_port() == port;
			++it_server)
		{serv = *it_server;}
	for (std::vector<location>::const_iterator
			it_loc = loc.begin(); it_loc != loc.end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(),
				it_loc->get_locations_path()) == 0)
			{
				path =  it_loc->get_root() + '/' +
					url.substr(it_loc->get_locations_path().length());
				return	file_exist(path) ? path : "";
			}
	}
	return "";
}

