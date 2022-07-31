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
	for (std::vector<Location>::const_iterator
			it_loc = server.get_location().begin();
			it_loc != server.get_location().end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(),
				it_loc->get_locations_path()) == 0)
			{
				return (true);
			}
	}
	return (false);
}

bool	url_redirected(std::string url, Server server)
{
	std::vector<std::vector<std::string> > red = server.get_redirections();

	for (std::vector<std::vector<std::string> >::const_iterator
			reds = red.begin(); reds != red.end(); ++reds)
	{
		for (std::vector<std::string>::const_iterator
				it_red = reds->begin(); it_red != reds->end(); ++it_red)
		{
			if (url.compare(0, it_red->length(), *it_red) == 0)
				return (true);
		}
	}
	return (false);
}

bool	method_is_allowed(std::string method, std::string url ,Server server)
{
	std::string path;
	bool		allowed = false;
	std::vector<Location> loc= server.get_location();

	for (std::vector<std::string>::const_iterator it_method =
		server.get_allowed_methods().begin();
		it_method != server.get_allowed_methods().end(); ++it_method)
	{
		if (method.compare(*it_method) == 0)
			allowed = true;
	}

	for (std::vector<Location>::const_iterator
			it_loc = loc.begin(); it_loc != loc.end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(),
				it_loc->get_locations_path()) == 0)
			{
				for (std::vector<std::string>::const_iterator it_method =
					it_loc->get_allow_methods().begin();
					it_method != it_loc->get_allow_methods().end(); ++it_method)
				{
					if (method.compare(*it_method) == 0)
						allowed = true;
				}
			}
	}
	return allowed;
}

std::string get_location_url(std::string url, Server server)
{
	std::string location;
	for (std::vector<Location>::const_iterator
			it_loc = server.get_location().begin();
			it_loc != server.get_location().end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(),
				it_loc->get_locations_path()) == 0)
			{
				return it_loc->get_locations_path() +
					url.substr(it_loc->get_locations_path().length());
			}
	}
	return "";
}

std::string get_redirection_url(std::string url, Server server)
{
	std::string location;
	for (std::vector<std::vector<std::string> >::const_iterator
			reds = server.get_redirections().begin();
			reds != server.get_redirections().end(); ++reds)
	{
		for (std::vector<std::string>::const_iterator
				it_red = reds->begin(); it_red != reds->end(); ++it_red)
		{
			if (url.compare(0, it_red->length(), *it_red) == 0)
				return *it_red + url.substr(it_red->length());
		}
	}
	return "";
}

bool	requested_file_in_root(std::string url, Server server)
{
	std::string path = server.get_root() + url;
	if (file_exist(path))
		return (true);
	return (false);
}

bool is_file(std::string url)
{
	struct stat s;
	if( stat(url.c_str(),&s) == 0 )
	{
	    if( s.st_mode & S_IFDIR )
			return (false);
	    return (true);
	}
	return (false);
}

bool Location_support_upload(std::string url, Server server)
{
	std::vector<Location> loc= server.get_location();
	for (std::vector<Location>::const_iterator
			it_loc = loc.begin(); it_loc != loc.end(); ++it_loc)
	{
		if (url.compare(0, it_loc->get_locations_path().length(),
				it_loc->get_locations_path()) == 0)
			{
				for (std::vector<std::string>::const_iterator it_method =
					it_loc->get_allow_methods().begin();
					it_method != it_loc->get_allow_methods().end(); ++it_method)
				{
					// if (it_method->compare("POST") == 0
					// 		&& it_loc->get_upload(url))
					// 	return (true);
				}
			}
	}
	return (false);
}
