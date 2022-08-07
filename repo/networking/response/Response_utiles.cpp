#include "Response_utiles.hpp"
#include "../../config/print.hpp"
#include <fstream>

std::vector<std::string> split_url(std::string url) {
	std::vector<std::string> ret;
	if (url.length() == 0) {
		return ret;
	}
	if (url[0] == '/') {
		url = &url.c_str()[1];
	}
	if (url.back() == '/') {
		url = url.substr(0, url.length() - 1);
	}
	return ret;
}

int str_matched(std::string str1, std::string str2)
{
	// PRINT_LINE_VALUE(str1);
	// PRINT_LINE_VALUE(str2);
	
	int i = 0;
	for (i = 0; i < str1.length() || i < str2.length() ||
			str1[i] != str2[i] ; i++);
	return i;

}

bool url_is_formated(std::string url)
{
	// std::string allowed ="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	// 						"abcdefghijklmnopqrstuvwxyz"
	// 						"0123456789-._~:/?#[]@!$&'()*+,;=";
	// for(std::string::iterator it = url.begin(); it != url.end(); ++it)
	// 	if (allowed.find(*it) == std::string::npos)
	// 		return false;
	return (true);
}

bool file_exist(std::string path)
{
	// println("file_exists path = ", path);
	struct stat st;
	int res = stat(path.c_str(), &st);
	// println("file_exists res = ", res);
	return (res == 0);
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
	new_url = "./autoindex/" + new_url + "index" + to_string(rand()) + ".html";
	std::cout << new_url << std::endl;
	std::ofstream file (new_url);
	file << "<!DOCTYPE html><html><body>";
	dr = opendir(url.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
			file << "<a href=\"" << en->d_name << "\">" << en->d_name << "</a><br>";
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
	bool was_matched = false;
	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;
	int allowed_method = 0;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (!std::strncmp(url.c_str(), location_str.c_str(), location_str.size())){
			was_matched = true;
			std::vector<std::string> location_meth = it_loc->get_allow_methods();
			for (std::vector<std::string>::iterator it = location_meth.begin(); it != location_meth.end(); it++)
				if (*it == method)
					allowed_method = true;
		}
	}
	if (!was_matched){
		std::vector<std::string> location_meth = server.get_allowed_methods();
			for (std::vector<std::string>::iterator it = location_meth.begin(); it != location_meth.end(); it++)
				if (*it == method)
					allowed_method = true;
	}
	return (allowed_method);
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

size_t  _getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

std::string get_error_page(int code, Server server)
{
	std::vector<std::vector<std::string> > _error_pages = server.get_error_pages();
	for (std::vector<std::vector<std::string> >::iterator it = _error_pages.begin(); it != _error_pages.end(); ++it)
	{
		if (it->at(0) == std::to_string(code))
		{
			if (file_exist(it->at(1)))
				return (it->at(1));
		}
	}
	return ("empty");
}

bool status_code_error(std::string status)
{
	if (status == "")
		return (false);
    int code = std::stoi(status);
    if (code == 501 || code == 405 || code == 400 || code == 414 || code == 404 || code == 413 || code == 405)
        return true;
    return false;
}

bool remove_all_folder_content(std::string path)
{
	DIR *dir;
	struct dirent *ent;
	bool ret = true;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				std::string file_path = path + "/" + ent->d_name;
				std::cout << "file_path : " << file_path << std::endl;
				std::cout << "ent->d_name : " << ent->d_name << std::endl;
				if (is_file(file_path))
				{
					// if (acce)
					if (remove(file_path.c_str()) != 0)
					{
						ret = false;
						continue;
					}
				}
				else
					remove_all_folder_content(file_path);
			}
		}
		closedir(dir);
		return (ret);
	}
	return (ret);
}

bool have_write_access_on_folder(std::string path)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				std::string file_path = path + "/" + ent->d_name;
				if (is_file(file_path))
				{
					if (access(file_path.c_str(), W_OK) != -1)
						return (true);
					else
						return (false);
				}
				else
					if (have_write_access_on_folder(file_path))
						return (true);
					else
						return (false);
			}
		}
		closedir(dir);
		return (true);
	}
	return (false);
}

bool location_support_upload(std::string url)
{
	bool ret = false;
	std::string file_name = remove_duplicate_slash(url + "/test");
	std::cout << file_name << std::endl;
	std::ofstream file(file_name);
	if (file.is_open())
	{
		ret = true;
		file.close();
		remove(file_name.c_str());
	}
	return (ret);
}

void upload_post(Request *request, Response *response, std::string upload_path)
{
	std::string file_name_in = request->getPath();
	for(std::string::reverse_iterator it = file_name_in.rbegin(); it != file_name_in.rend(); ++it)
	{
		if (*it == '/')
		{
			file_name_in.erase(file_name_in.begin(), it.base());
			break;
		}
	}
	if (file_name_in == "")
	{
		std::string file_name_in = request->getPath();
		for(std::string::reverse_iterator it = file_name_in.rbegin(); it != file_name_in.rend(); ++it)
		{
			if (*it == '/')
			{
				file_name_in.erase(file_name_in.begin(), it.base());
				break;
			}
		}
	}
	std::cout << "file_name_in : " << file_name_in << std::endl;
	std::string file_name_out = remove_duplicate_slash(upload_path + "/" + file_name_in);
	int fd_in = open(request->getBodyFileName().c_str(), O_RDWR);
	int fd_out = open(file_name_out.c_str(), O_CREAT| O_RDWR, 0666);
	if (fd_in < 0 || fd_out < 0)
	{
		response->setStatus(" 500 Internal Server Error\r\n");
		return ;
	}
	char *buffer = (char *)malloc(sizeof(char) * (_BUFFER_SIZE + 1));
	int n = -1;
	while ((n = read(fd_in, buffer, _BUFFER_SIZE)) > 0)
	{
		buffer[n] = '\0';
		write(fd_out, buffer, n);
	}
	free(buffer);
	close(fd_in);
	close(fd_out);
}

bool Location_have_cgi(std::string url)
{
	for (std::string::reverse_iterator it = url.rbegin(); it != url.rend(); ++it)
	{
		if (*it == '.')
		{
			url.erase(url.begin(), it.base());
			break;
		}
	}
	if (url == "php" || url == "PHP" || url == "PY" || url == "py")
		return (true);
	return (false);
}

std::pair<std::string, std::string> _cgi_ret(std::string url)
{
	std::pair<std::string, std::string> ret;
	ret.first = "";
	ret.second = "";
	return (ret);
}

bool check_path_hierarchy(std::string root, std::string path)
{
	char actualpath_root [PATH_MAX+1];
	char actualpath_req [PATH_MAX+1];
	char* _root = realpath(root.c_str(), actualpath_root);
	char* _path = realpath(path.c_str(), actualpath_root);
	if (!(_root && _path))
		return (false);
	return (std::strncmp(actualpath_root, actualpath_root, strlen(actualpath_root)) == 0);
}

bool check_auto_index(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();

	std::vector<Location>::const_iterator it_loc = location.begin();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;
	bool auto_index = false;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (!std::strncmp(url.c_str(), location_str.c_str(), location_str.size()))
		{
			if (str_matched(location_str, location_path) > location_path_matched)
				auto_index = it_loc->get_autoindex();
		}
	}
	// if (location_path.empty())
	// 	return (server.get_autoindex());
	return auto_index;
}