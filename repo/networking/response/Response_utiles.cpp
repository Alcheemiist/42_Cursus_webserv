#include "Response_utiles.hpp"
#include "../../config/print.hpp"

int str_matched(std::string str1, std::string str2)
{
	int i = 0;
	for (i = 0; i < str1.length() || i < str2.length() ||
			str1[i] != str2[i] ; i++);
	return i;
}

bool url_is_formated(std::string url)
{
	std::string allowed ="ABCDEFGHIJKLMNOPQRSTUVWXYZ\
							abcdefghijklmnopqrstuvwxyz\
							0123456789-._~:/?#[]@!$&'()*+,;=";
	for(std::string::iterator it = url.begin(); it != url.end(); ++it)
		if (allowed.find(*it) == std::string::npos)
			return false;
	return (true);
}

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
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (url.substr(0, location_str.size()) == location_str && it_loc->get_root() != "")
		{
			if (str_matched(location_str, location_path) > location_path_matched)
			{
				location_path = location_str;
				location_path_matched = str_matched(location_str, location_path);
				location_matched = *it_loc;
			}
		}
	}
	if (location_path_matched)
		location_path = url.replace(0, location_path.size(), location_matched.get_root());
	else
		location_path = server.get_root() + url;
	return (file_exist(location_path));
}

bool	url_redirected(std::string url, Server server)
{
	std::vector<std::vector<std::string> > red = server.get_redirections();
	std::string redirection_str;
	std::string redirection_path = "";
	int redirection_path_matched = 0;

	for(std::vector<std::vector<std::string> >::iterator it = red.begin(); it != red.end(); ++it)
	{
		redirection_str = it->at(0);
		if ((*it)[1].back() != '/')
				redirection_str += '/';
		if (url.substr(0, redirection_str.size()) == redirection_str)
		{
			if (str_matched(redirection_str, redirection_path) > redirection_path_matched)
			{
				redirection_path = (*it)[2];
				redirection_path_matched = str_matched(redirection_str, redirection_path);
			}
		}
	}
	if (redirection_path_matched)
		return (true);
	return (false);
		// redirection_path = url.replace(0, redirection_path.size(), redirection_path);
}

bool	method_is_allowed(std::string method, std::string url ,Server server)
{
	bool allowed = false;
	std::vector<std::string> _allowed_methods = server.get_allowed_methods();
	for (std::vector<std::string>::iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); ++it)
		if (*it == method)
			allowed = true;
	if (!allowed)
		return (false);

	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (url.substr(0, location_str.size()) == location_str && it_loc->get_root() != "")
		{
			if (str_matched(location_str, location_path) > location_path_matched)
			{
				location_path = location_str;
				location_path_matched = str_matched(location_str, location_path);
				location_matched = *it_loc;
			}
		}
	}
	_allowed_methods = it_loc->get_methods();
	for (std::vector<std::string>::iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); ++it)
		if (*it == method)
			return true;
	if (!allowed)
		return (false);


	// (void)method;
	// (void)url;
	// std::string path;
	// bool		allowed = false;
	// std::vector<Location> loc = server.get_location();

	// for (std::vector<std::string>::const_iterator it_method =
	// 	server.get_allowed_methods().begin();
	// 	it_method != server.get_allowed_methods().end(); ++it_method)
	// {
	// 	if (method.compare(*it_method) == 0)
	// 		allowed = true;
	// }

	// for (std::vector<Location>::const_iterator
	// 		it_loc = loc.begin(); it_loc != loc.end(); ++it_loc)
	// {
	// 	if (url.compare(0, it_loc->get_locations_path().length(),
	// 			it_loc->get_locations_path()) == 0)
	// 		{
	// 			for (std::vector<std::string>::const_iterator it_method =
	// 				it_loc->get_allow_methods().begin();
	// 				it_method != it_loc->get_allow_methods().end(); ++it_method)
	// 			{
	// 				if (method.compare(*it_method) == 0)
	// 					allowed = true;
	// 			}
	// 		}
	// }
	// return allowed;
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
	std::vector<std::vector<std::string> > redirections = server.get_redirections();
	std::string location;
	for (std::vector<std::vector<std::string> >::const_iterator
			reds = redirections.begin();
			reds != redirections.end(); ++reds)
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

char    *readAllFile(char *path)
{
    char buffer[DATA_BUFFER_SIZE];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);
    int max_size = getFileSize(path);
    int fd = open(path, O_RDWR);
    size_t read_size = 0;

    if (fd < 0)
        throw std::runtime_error("Error opening file");
    else
        for (int i = 0; i < max_size; i += DATA_BUFFER_SIZE)
        {
            int size = read(fd, buffer, DATA_BUFFER_SIZE);
            for (int i = 0; i < size; i++)
                return_buffer[i + read_size] = buffer[i];
            if (size == 0)
                break;
            if (size < 0)
                throw std::runtime_error("Error reading file");
            read_size += size;
        }
        return_buffer[max_size] = '\0';
    return return_buffer;
}

size_t  _getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

std::vector<char> read_by_vector(char *path, Response *response)
{
    int fd = open(path, O_RDWR), n = -1;
    char *_buffer = (char *)malloc(sizeof(char) * (response->getbody_file_size() + 1));
    std::vector<char> buffer;

    if (fd < 0)
        perror("Error opening file");
    else
    {
        for (int i = 0; i < response->getbody_file_size(); i += n)
        {
            if ((n = read(fd, _buffer + i, (1024 * 16))) < 0)
                break;
        }
        buffer.insert(buffer.begin(), _buffer, _buffer + response->getbody_file_size());
    }
    close(fd);
    return buffer;
}

