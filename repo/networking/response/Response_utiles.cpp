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

std::string url_matched(std::string url, int port, const std::vector<Server> servers)
{
	Server serv;
	std::string path;
	std::vector<Location> loc= serv.get_location();

	for (std::vector<Server>::const_iterator it_server = servers.begin();
			it_server != servers.end() || it_server->get_listen_port() == port;
			++it_server)
		{serv = *it_server;}
	for (std::vector<Location>::const_iterator
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

std::string url_redirected(std::string url, int port, const std::vector<Server> redirections)
{
	Server serv;
	std::string path;
	std::vector<std::vector<std::string> > red = serv.get_redirections();
	// std::vector<Location> loc= serv.get_location();

	for (std::vector<Server>::const_iterator it_server = redirections.begin();
			it_server != redirections.end() || it_server->get_listen_port() == port;
			++it_server)
		{serv = *it_server;}

	for (std::vector<std::vector<std::string> >::const_iterator
			reds = red.begin(); reds != red.end(); ++reds)
	{
		if (url.compare(0, (*reds)[0].length(), (*reds)[0]) == 0)
		{
			path =  (*reds)[1] + '/' +
				url.substr((*reds)[1].length());
			return	path;
		}
	}
	return "";
}

// bool	method_is_allowed(std::string method, const std::vector<Server> servres)
// {
// 	Server serv;
// 	std::string path;
// 	std::vector<std::vector<std::string> > red = serv.get_redirections();
// 	// std::vector<Location> loc= serv.get_location();

// 	for (std::vector<Server>::const_iterator it_server = servres.begin();
// 			it_server != servres.end() || it_server->get_listen_port() == port;
// 			++it_server)
// 		{serv = *it_server;}

// 	if (serv.get_allowed_methods().find(method) != serv.get_allowed_methods().end())
// 		return true;

// 	for (std::vector<std::vector<std::string> >::const_iterator
// 			reds = red.begin(); reds != red.end(); ++reds)
// 	{
// 		if (url.compare(0, (*reds)[0].length(), (*reds)[0]) == 0)
// 		{
// 			path =  (*reds)[1] + '/' +
// 				url.substr((*reds)[1].length());
// 			return	path;
// 		}
// 	}
// 	return "";
// }

