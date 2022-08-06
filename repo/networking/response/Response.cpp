#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"
#include "../../config/print.hpp"
#include "../../config/utils.hpp"
#include <cstring>

#define DELIMITER "\r\n"

Response  response(Request *request, ParseConfig *config, int index_server)
{
    std::string header_str = "";
    std::cout << blue << "********** { Procces Response } ***********************" << def << std::endl;
    Response response;
    response.setpath("empty");
    std::string s;

    /* is_req_well_formed() */
    response.setStatus("");
    s = response.setStatus(request, config->get_server_vect()[index_server]);
    if (status_code_error(response.get_status()) && request->getMethod() != "POST")
        ERRORresponse(request, &response, config, index_server);
    else if (!(request->getMethod().compare("GET")))
        s = GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("DELETE") == 0)
        s = DELETEresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        s = POSTresponse(request, &response, config, index_server);
    std::cout << "Requested path : " << response.get_location() << std::endl;
    std::cout << "Redirection path : " << response.get_redirection() << std::endl;
    if (!response.get_redirection().empty())
        response.setContentType(response.get_redirection());
    else
        response.setContentType(response.get_location());

    std::cout << blue << "********** {End Procces Response } ******************" << def << std::endl ;

    // response.setHeader(header_str);

    std::cout << blue << "********** { Response Header } ***********************" << def << std::endl;
    std::cout << header_str << std::endl;
    response.setpath(s);
    std::cout << "body_path : " << response.getpath() << std::endl;
    std::cout << "body_path : " << s << std::endl;

    std::cout << blue << "********** { End Response Header } ***********************" << def << std::endl;

    std::cout << blue << "********** { Response Body } ***********************" << def << std::endl;
    println("file = ", response.getpath());
    // if (response.get_status() != " 200 OK\r\n")
    //     response.setpath("empty");
    //    exit(0);
    return response;
}

std::string Response::setStatus(Request *request, Server server)
{
    std::string code_redirection;
    std::string code_status_file;
    this->init_location(request->geturl(),server);
    this->init_redirection(request->geturl(), server, code_redirection);
	if (!request->get_is_formated())
	{
        
		if (request->get_transfer_encoding().size() > 0 && request->get_transfer_encoding() != "chunked")
        {
			this->status = " 501 NOT IMPLEMENTED\r\n";
            code_status_file = get_error_page(501, server);
        }
		else if (!request->get_transfer_encoding().size() && request->getcontent_length() <= 0 &&  request->getMethod() == "POST")
        {
            this->status = " 400 BAD REQUEST\r\n";
            code_status_file = get_error_page(400 , server);
        }
		else if (!url_is_formated(request->geturl()))
        {
            this->status = " 400 BAD REQUEST\r\n";
            code_status_file = get_error_page(400 , server);
        }
        else if (request->geturl().length() > MAX_URL_LENGTH)
		{
        	this->status = " 414 REQUEST-URI TOO LARGE\r\n";
            code_status_file = get_error_page(414, server);
        }
        else if ( request->get_body_length() > server.get_client_max_body_size())
        {
        	this->status = " 413 REQUEST ENTITY TOO LARGE\r\n";
            code_status_file = get_error_page(413, server);
        }
    }
    if (!method_is_allowed(request->getMethod(), request->geturl(),  server))
    {
        this->status = " 405 METHOD NOT ALLOWED\r\n";
        code_status_file = get_error_page(413, server);
    }
    else if (!get_redirection().empty())
    {
        this->status = code_redirection;
        code_status_file = get_error_page(std::atoi(code_redirection.c_str()), server);
    }
    else if (!file_exist(get_location()))
    {
        this->status = " 404 NOT FOUND\r\n";
        code_status_file = get_error_page(404, server);
    }
    return code_status_file;
}

void Response::init_location(std::string url, Server server)
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
		this->requested_path = remove_duplicate_slash(url.replace(0, location_path.size(), location_matched.get_root()));
	else
		this->requested_path = remove_duplicate_slash(server.get_root() + url);
}

std::string get_status_code(int n)
{
    std::string code = "";
    switch (n)
    {
        case 300:
            code = " 300 MULTIPLE CHOICES\r\n";
            break;
        case 302:
            code = " 302 FOUND\r\n";
            break;
        case 303:
            code = " 303 SEE OTHER\r\n";
            break;
        case 304:
            code = " 304 NOT MODIFIED\r\n";
            break;
        case 305:
            code = " 305 USE PROXY\r\n";
            break;
        case 307:
            code = " 307 TEMPORARY REDIRECT\r\n";
            break;
        case 308:
            code = " 308 PERMANENT REDIRECT\r\n";
            break;
        default :
            code = " 301 MOVED PERMANENTLY\r\n";
    }
    return code;
}

void	Response::init_redirection(std::string url, Server server, std::string &statusLine)
{
	std::vector<std::vector<std::string> > red = server.get_redirections();
	std::string redirection_str;
	std::string _redirection_path = "";
	int redirection_path_matched = 0;
    std::string path_absol = "";

	for(std::vector<std::vector<std::string> >::iterator it = red.begin(); it != red.end(); ++it)
	{
		redirection_str = it->at(0);
		if (redirection_str.back() == '/')
				redirection_str.pop_back();
		if (url.substr(0, redirection_str.size()) == redirection_str)
		{
			if (str_matched(redirection_str, _redirection_path) > redirection_path_matched)
			{
                println("it02 = ", it[0][2]);
                if (std::isdigit(it[0][2][0]))
                {
                    statusLine = get_status_code(to_int(it[0][2]));
                }
                else if (it[0][2] == "temporary")
                {
                    statusLine = get_status_code(307);
                }
                else
                {
                    statusLine = get_status_code(308);
                }
                println("status = ", statusLine);
                _redirection_path = (*it)[1];
				redirection_path_matched = str_matched(redirection_str, _redirection_path);
                if (std::strncmp(_redirection_path.c_str(), "http", 4) == 0)
                    path_absol = _redirection_path;
                else
                    path_absol = server.get_root() + _redirection_path;
			}
		}
	}
	if (redirection_path_matched)
    {
        if (path_absol.length() > 0)
            this->redirection_path = path_absol;
        else
            this->redirection_path = url.replace(0, _redirection_path.size(), _redirection_path);
    }
    else
        this->redirection_path = "";
}

void Response::setContentType(std:: string s)
{
    std::string s1 = s.substr(s.find_last_of(".") + 1);
    if (s1 == "html" || s1 == "htm")
        this->contentType = "text/html";
    else if (s1 == "css")
        this->contentType = "text/css";
    else if (s1 == "js")
        this->contentType = "application/javascript";
    else if (s1 == "jpg")
        this->contentType = "image/jpeg";
    else if (s1 == "png")
        this->contentType = "image/png";
    else if (s1 == "gif")
        this->contentType = "image/gif";
    else if (s1 == "ico")
        this->contentType = "image/x-icon";
    else if (s1 == "svg")
        this->contentType = "image/svg+xml";
    else if (s1 == "mp3")
        this->contentType = "audio/mpeg";
    else if (s1 == "mp4")
        this->contentType = "video/mp4";
    else if (s1 == "ogg")
        this->contentType = "audio/ogg";
    else if (s1 == "ogv")
        this->contentType = "video/ogg";
    else if (s1 == "wav")
        this->contentType = "audio/wav";
    else if (s1 == "webm")
        this->contentType = "video/webm";
    else if (s1 == "txt")
        this->contentType = "text/plain";
    std::cout << red << "- Set Content-Type : " << s1 << green << " " << s1 << def << std::endl;
};

std::vector<char> Response::get_buffer()
{
    std::vector <char> buffer;

    int fd = open(this->body_file_path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        std::cout << "open file error";
    }
    int size = lseek(fd, maxBufferLenght, SEEK_SET);
    if (size < 0)
        std::cout << "lseek error";
    char *buf = (char *)malloc((_BUFFER_SIZE) + 1);
    int read_size = read(fd, buf, (_BUFFER_SIZE));
    if (read_size < 0)
        std::cout << "read error";
    maxBufferLenght += read_size;
    for (int i = 0; i < read_size; i++)
        buffer.push_back(buf[i]);
    free(buf);
    close(fd);
    if (maxBufferLenght >= getFileSize(body_file_path.c_str()))
        this->is_complete = true;
    else 
        this->is_complete = false;
    std::cout << red << "- Read_size(" << _BUFFER_SIZE << ")  : " << read_size << " maxBuffer : " << maxBufferLenght << " file path size : " << getFileSize(body_file_path.c_str()) << def << std::endl;
    return buffer;
};

std::string Response::getHeader() 
{
    std::string res;


    if (is_cgi)
    {
        std::string tmp;
        std::string path = this->body_file_path;

        if (path.size() > 0)
        {
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0)
                std::cout << "error open file cgi " << std::endl;
            int size = lseek(fd, 0, SEEK_END);
            if (size < 0)
                std::cout << "error lseek file cgi " << std::endl;
            
            char *buf = (char *)malloc(2);
            int max_size = read(fd, buf, 1);
            if (max_size < 0)
                std::cout << "error read file cgi " << std::endl;
            int curr_size = 0, read_size = 0;
            int flag_delimiter = 0;
            while (read_size > 0)
            {
                //tmp += buf;
                read_size = read(fd, buf, 1);
                if (read_size < 0)
                    std::cout << "error read file cgi " << std::endl;
                //
                buf[read_size] = '\0';
                curr_size += read_size;
                //
                if (buf[0] == '\r' && flag_delimiter == 0)
                    flag_delimiter = 1;
                else if (buf[0] == '\n' && flag_delimiter == 1)
                    flag_delimiter = 2;
                else if (flag_delimiter == 2 && buf[0] == '\r')
                    flag_delimiter = 3;
                else if (flag_delimiter == 3 && buf[0] == '\n')
                    flag_delimiter = 4;
                else 
                    flag_delimiter = 0;
                //     
                if (buf)
                    tmp.append(buf);
                if (flag_delimiter == 4)
                    break;
                //
            }
            free(buf);
            close(fd);
            std::cout << red << "- CGI Header : {" << tmp << "}" << def << std::endl;
        }
        else 
        {
            // path not found
        }

    }
    else 
    {


        // status line
        res.append(version);
        res.append(status);
        // headers
        if (this->contentType.length()){
            res.append("Content-Type: ");
            res.append(this->contentType);
            res.append("\r\n");
        }
        if (!get_redirection().empty())
            res.append("Location: " + get_redirection() + "\r\n");
        size_t tt = getFileSize(body_file_path.c_str());
        if (tt && tt != (size_t)-1)
        {
            res.append("Content-Length: ");
            res.append(std::to_string(tt));
            res.append("\r\n");
        }
        res.append("server: alchemist\r\n");
        res.append("\r\n");
        setHeader(res);
    
    }
    // 
    return res;
};

std::string ERRORresponse(Request *request, Response *response, ParseConfig *config, int server_index)
{
    std::string body_f = "";
    body_f = get_error_page(403,  config->get_server_vect()[server_index]);
    response->setpath(body_f);
    return body_f;
}

std::string DELETEresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
{
    std::string body_f = "empty";
    if(requested_file_in_root(response->get_location()))
    {
        if (is_file(response->get_location()))
        {
            // if (Location_have_cgi(response->get_location()))
            // {
            //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(response->get_location());
            //     response->setStatus(cgi_pair.first);
            //     response->setpath(cgi_pair.second);
            //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
            // }
            // else
            // {
                remove(response->get_location().c_str());
                response->setStatus(" 204 No Content\r\n"); 
                get_error_page(204, config->get_server_vect()[index_server]);
            // }
        }
        else
        {
            if (response->get_location().back() == '/')
            {
                std::string index_path = response->get_index(response->get_location(), config->get_server_vect()[index_server]);
                // if (Location_have_cgi(index_path))
                // {
                //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(index_path);
                //     response->setStatus(cgi_pair.first);
                //     response->setpath(cgi_pair.second);
                //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
                // }
                // else
                // {
                    if(remove_all_folder_content(response->get_location()))
                    {
                        response->setStatus(" 204 No Content\r\n");
                        get_error_page(204, config->get_server_vect()[index_server]);
                    }
                    else
                    {
                        if (have_write_access_on_folder(response->get_location()))
                        {
                            response->setStatus(" 500 Internal Server Error\r\n");
                            get_error_page(500, config->get_server_vect()[index_server]);
                        }
                        else
                        {
                            response->setStatus(" 403 Forbidden\r\n");
                            get_error_page(403, config->get_server_vect()[index_server]);
                        }
                    }
                // }
            }
            else
            {
                response->setStatus(" 409 Conflict\r\n");
                get_error_page(409, config->get_server_vect()[index_server]);
            }
        }
    }
    else
    {
        response->setStatus(" 403 Forbidden\r\n");
        get_error_page(403, config->get_server_vect()[index_server]);
    }
    return "empty";
}

std::string  POSTresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
{
    (void)request;
    (void)config;
    std::string body_f = "empty";
    std::string path_upload_file;

    if(location_support_upload(config->get_server_vect()[index_server].get_upload_path()))
    {
        upload_post(request, response, config->get_server_vect()[index_server].get_upload_path());
        body_f = get_error_page(201, config->get_server_vect()[index_server]);
        response->setStatus(" 201 Created\r\n");
    }
    else
    {
        if (is_file(path_upload_file))
        {
            // if (Location_have_cgi(path_upload_file))
            // {
            //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(path_upload_file);
            //     response->setStatus(cgi_pair.first);
            //     response->setpath(cgi_pair.second);
            //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
            // }
            // else
            // {
                body_f = get_error_page(403, config->get_server_vect()[index_server]);
                response->setStatus(" 403 Created\r\n");
                body_f = get_error_page(403, config->get_server_vect()[index_server]);
            // }
        }
        else
        {
            if (response->get_location().back() == '/')
            {
                std::string index_path = response->get_index(response->get_location(), config->get_server_vect()[index_server]);
                if (file_exist(index_path))
                {
                    // if (Location_have_cgi(index_path))
                    // {
                    //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(index_path);
                    //     response->setStatus(cgi_pair.first);
                    //     response->setpath(cgi_pair.second);
                    //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
                    // }
                    // else
                    // {
                        body_f = get_error_page(403, config->get_server_vect()[index_server]);
                        response->setStatus(" 403 Created\r\n");
                        body_f = get_error_page(403, config->get_server_vect()[index_server]);
                    // }
                }
                else
                {
                    body_f = get_error_page(403, config->get_server_vect()[index_server]);
                    response->setStatus(" 403 Created\r\n");
                }
            }
            else
            {
                response->set_redirection(response->get_location() + "/");
                std::cout << "redirection to " << response->get_redirection() << std::endl;
                response->setStatus(" 301 MOVED PERMANENTLY\r\n");
                body_f = get_error_page(301,  config->get_server_vect()[index_server]);
                response->setpath(body_f);
            }
        }
        body_f = get_error_page(403, config->get_server_vect()[index_server]);
    }
    return body_f;
    std::cout << "im doing post response\n";
}

std::string  GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    (void)request;
    /////// main process to set a good response : set mandatory headers + set path of file to send
    std::string body_f = "empty";
    if(!response->get_status().empty())
        return body_f;
    if(requested_file_in_root(response->get_location()))
    {
        if (is_file(response->get_location()))
        {
            // if (Location_have_cgi(response->get_location())) // if location have cgi
            // {
            //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(response->get_location());
            //     response->setStatus(cgi_pair.first);
            //     response->setpath(cgi_pair.second);
            //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
            // }
            // else 
            // {
                response->setpath(response->get_location());
                response->setStatus(" 200 OK\r\n");
                body_f = response->getpath();
            // }
                
        }
        else // if you request a directory
        {
            if (response->get_location().back() == '/') // uri have / at the end
            {
                std::string index_path = response->get_index(response->get_location(), config->get_server_vect()[index_server]);
                if (file_exist(index_path)) //have index file
                {
                    // if (Location_have_cgi(index_path)) // location have cgi
                    // {
                    //     std::pair<std::string, std::string> cgi_pair = _cgi_ret(index_path);
                    //     response->setStatus(cgi_pair.first);
                    //     response->setpath(cgi_pair.second);
                    //     body_f = get_error_page(std::atoi(cgi_pair.first.c_str()), config->get_server_vect()[index_server]);
                    // }
                    // else // location have note cgi
                    // {
                        response->set_index(index_path);
                        response->setpath(response->get_index());
                        response->setStatus(" 200 OK\r\n");
                        body_f = index_path;
                    // }
                }
                else
                {
                    if(config->get_server_vect()[index_server].get_autoindex())
                    {
                        body_f = generate_auto_index(response->get_location());
                        response->setpath(body_f);
                        response->setStatus(" 200 OK\r\n");
                    }
                    else
                    {
                        response->setStatus(" 403 FORBIDDEN\r\n");
                        body_f = get_error_page(403,  config->get_server_vect()[index_server]);
                        response->setpath(body_f);
                    }
                }
            }
            else //uri have not a / at the end
            {
                response->set_redirection(request->getPath() + "/");
                std::cout << "redirection to " << response->get_redirection() << std::endl;
                response->setStatus(" 301 MOVED PERMANENTLY\r\n");
                body_f = get_error_page(301,  config->get_server_vect()[index_server]);
                response->setpath(body_f);
            }
        }
    }
    else // requested resource not in root
    {
        response->setStatus(" 404 NOT FOUND\r\n");  //requested resource not found
        body_f = get_error_page(301,  config->get_server_vect()[index_server]);
        response->setpath(body_f);
    }

    return body_f;
}

std::string Response::get_index(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
    std::vector<std::string> index_file = server.get_index();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (url.substr(0, location_str.size()) == location_str)
		{
			if (str_matched(location_str, location_path) > location_path_matched)
			{
				std::vector<std::string> indexVec = it_loc->get_index();
				for(std::vector<std::string>::iterator it = indexVec.begin();
                        it != indexVec.end(); ++it)
				{
					if (file_exist(remove_duplicate_slash(url + "/" + *it)))
					{
						return (remove_duplicate_slash(url + "/" + *it));
					}
				}
				location_path = location_str;
				location_path_matched = str_matched(location_str, location_path);
				location_matched = *it_loc;
			}
		}
	}
	for(std::vector<std::string>::iterator it = index_file.begin(); it != index_file.end(); ++it)
	{
		if (file_exist(remove_duplicate_slash(url + "/" + *it)))
		{
			return (url + "/" + *it);
		}
	}
	return "";
}

