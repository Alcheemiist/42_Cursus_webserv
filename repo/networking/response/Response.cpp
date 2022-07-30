#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"

bool Response::url_is_formated(std::string url)
{
	std::string allowed ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
	for(std::string::iterator it = url.begin(); it != url.end(); ++it)
		if (allowed.find(*it) == std::string::npos)
			return false;
	return (true);
}

void Response::setStatus(Request *request, Server server)
{
	if (!request->get_is_formated())
	{
		if (request->get_transfer_encoding() != "" &&
                request->get_transfer_encoding() != "chunked")
			this->status = "501 NOT IMPLEMENTED";
		else if (request->get_transfer_encoding() == "" &&
			    request->getcontent_length() == -1 &&
                request->getMethod() == "POST")
			this->status = "400 BAD REQUEST";
		else if (url_is_formated(request->geturl()))
			this->status = "400 BAD REQUEST";
		else if (request->geturl().length() > MAX_URL_LENGTH)
			this->status = "414 REQUEST-URI TOO LARGE";
		// else if (request->body_length() > request->getcontent_length())
		// else if (check_max_body_length(request->getbody_length(),
        //             request->getcontent_length()))
		// 	this->status = "413 REQUEST ENTITY TOO LARGE";
	}
	else
	{
		if (get_matched_location_for_request_uri(request->geturl(), server))
			this->status = "404 NOT FOUND";
		else if (url_redirected(request->geturl(), server))
			this->status = "301 MOVED PERMANENTLY";
		else if (!method_is_allowed(request->getMethod(), request->geturl(),
                    server))
            this->status = "405 METHOD NOT ALLOWED";
	}
    std::cout << "- Set Status : " << this->status << std::endl;
}

void Response::setContentType(char *path)
{
    std::string s(path);
    std::string s1 = s.substr(s.find_last_of(".") + 1);
    if (s1 == "html")
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
    std::cout << red << "- Set Content-Type : " << s << green << " " << s1 << def << std::endl;
};

char *readAllFile(char *path)
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

size_t _getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
}

void DELETEresponse()
{
    std::cout << "im doing delete response\n";
}

void POSTresponse()
{
    std::cout << "im doing post response\n";
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

void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
    if (!request->getPath().empty())
    {
        char *path = (char *)malloc(sizeof(char) * (1000));
        std::cout << B_red << "root path = {" << config->get_server_vect()[index_server].get_root().c_str() << "}" << B_def << std::endl;
        strcpy(path, config->get_server_vect()[index_server].get_root().c_str());
        struct stat st;
        if (request->getPath() == "/")
            strcpy(path + (strlen(path)), "index.html");
        else
            strcpy(path + (strlen(path) - 1), request->getPath().c_str());

        stat(path, &st);
        char s2[50];
        
        if (st.st_size > 0 && open(path, O_RDONLY) > 0)
        {
            strcpy(s2, "200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(path);
            response->setbody_file_size(getFileSize(path));
            // response->setBody(read_by_vector(path, response));
            response->setpath(path);
            std::cout << B_blue << "getFileSize(path): " << getFileSize(path) << B_def << std::endl;

        }
        else
        {
            response->set_requestFuckedUp(true);

            strcpy(s2, "404 Not Found\r\n");
            char _path[100] = "./errorsPages/404/404.html";
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(_path);
            response->setbody_file_size(getFileSize(_path));
            // response->setBody(read_by_vector(path, response));
            response->setpath(_path);
            std::cout << B_blue << "getFileSize(path): " << getFileSize(_path) << B_def << std::endl;

        }
        free(path);
    }
}

Response response(int new_socket, Request *request, ParseConfig *config, int index_server)
{
    (void)new_socket;
    Response response;

    std::cout << blue << "********** { Procces Response } ***********************" << def << std::endl;
	
    response.setStatus(request, config->get_server_vect()[index_server]);
    
    if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        ERRORresponse(request, &response);
    
    std::cout << blue << "********** {End Procces Response } ******************" << def << std::endl ;
    return response;
}
