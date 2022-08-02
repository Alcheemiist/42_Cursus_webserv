#include "Response_utiles.hpp"
#include "../../config/print.hpp"

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
	std::string location_path;
	int location_path_size = 0;
	Location loca;



	// check if path exist in root directory
	// if (url == "/")
	// 	return true;

	// if (is_file_exist(server.get_root() + (url)))
	// 	return true;

	for (; it_loc != location.end(); it_loc++)
	{
		std::cout << "{ " << std::endl;
		std::cout << it_loc->get_locations_path().c_str() << 	std::endl;
		if (!it_loc->get_locations_path().compare(0, url.size(), url))
		{
			if (it_loc->get_locations_path().size() > location_path_size)
			{
				location_path_size = it_loc->get_locations_path().size();
				loca = *it_loc;
			}
		}
		if (location_path_size)
			location_path = url.replace(0, location_path_size, loca.get_root());
		else
			location_path = url;

	}
	while(1);
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
	(void)method;
	(void)url;
	std::string path;
	bool		allowed = false;
	std::vector<Location> loc = server.get_location();

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
