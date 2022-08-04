#include "Response_utiles.hpp"
#include "../../config/print.hpp"
#include <fstream>


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

std::string remove_duplicate_slash(std::string str)
{
	std::string new_str = "";
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == '/' && *(it + 1) == '/')
			continue;
		new_str += *it;
	}
	return new_str;
}

std::string generate_auto_index(std::string url)
{
	DIR *dr;
	struct dirent *en;
	std::string new_url = "";
	for (std::string::iterator it = url.begin(); it != url.end(); ++it)
	{
		if (*it != '/' && *it != '.')
			new_url += *it;
	}
	new_url = new_url + "index.html";
	std::cout << new_url << std::endl;
	std::ofstream file (new_url);
	file << "<!DOCTYPE html><html><body>";
	dr = opendir(url.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
			file << "<a href=\"" <<en->d_name << "\">" << en->d_name << "</a><br>";
	}
	file << "</body></html>";
    closedir(dr);
	file.close();
	std::string cmd = "cat " + new_url;
	system(cmd.c_str());
	return new_url;
}

bool	method_is_allowed(std::string method, std::string url ,Server server)
{
	bool allowed = false;
	std::cout << "method : " << method << std::endl;
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
	std::vector<std::string> location_matched;

	while (it_loc != location.end())
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
				for (std::vector<std::string>::iterator it = it_loc->get_allow_methods().begin(); it != it_loc->get_allow_methods().end(); ++it)
				{
					if (*it == method)
					{
						allowed = true;
						break;
					}
					allowed = false;
				}
			}
		}
		it_loc++;
	}
	return (allowed);
}

bool	requested_file_in_root(std::string url)
{
	return(file_exist(url));
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
