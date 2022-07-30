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

bool get_matched_location_for_request_uri(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
	while (it_loc != server.get_location().end())
	{
	
		if (url.compare(0, it_loc->get_locations_path().length(),
						it_loc->get_locations_path()) == 0)
		{
			return (true);
		}
		it_loc++;
	}
	return (false);
}

bool url_redirected(std::string url, Server server)
{
	std::vector<std::vector<std::string> > red = server.get_redirections();

	for (std::vector<std::vector<std::string> >::const_iterator
			 reds = red.begin();
		 reds != red.end(); ++reds)
	{
		for (std::vector<std::string>::const_iterator
				 it_red = reds->begin();
			 it_red != reds->end(); ++it_red)
		{
			if (url.compare(0, it_red->length(), *it_red) == 0)
				return (true);
		}
	}
	return (false);
}

bool method_is_allowed(std::string method, std::string url, Server server)
{
	std::string path;
	bool allowed = false;
	std::vector<Location> loc = server.get_location();

	std::vector<std::string> methods = server.get_allowed_methods();
	std::vector<std::string>::const_iterator it_method = methods.begin();
	while ( it_method != server.get_allowed_methods().end())
	{
		if (method.compare(*it_method) == 0)
			allowed = true;
		it_method++;
	}

	for (std::vector<Location>::const_iterator
			 it_loc = loc.begin();
		 it_loc != loc.end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(), it_loc->get_locations_path()) == 0)
		 {
			std::vector<std::string> methods = it_loc->get_allow_methods();
			std::vector<std::string>::const_iterator it_method = methods.begin();
			while ( it_method != methods.end())
			{
				if (method.compare(*it_method) == 0)
					allowed = true;
				++it_method;
			}
		}
	}
	return allowed;
}
