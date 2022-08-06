#include "../elements.hpp"
#include "../../config/print.hpp"
#include "../../config/utils.hpp"
#include <algorithm>
#include <string.h>

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

size_t getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

void Request::show()
{
    std::cout << def << red << "--------------- Request ----------------- " << def << std::endl;
    std::cout << "method: " << this->_method << std::endl;
    std::cout << "path: " << this->_path << std::endl;
    std::cout << "version: " << this->_version << std::endl;
    std::cout << "connection: " << this->_connection << std::endl;
    std::cout << "accept: " << this->_accept << std::endl;
    std::cout << "accept-encoding: " << this->_accept_encoding << std::endl;
    std::cout << "accept-language: " << this->_accept_language << std::endl;
    std::cout << "content-length: " << this->_content_length << std::endl;
    std::cout << "content-type: " << this->_content_type << std::endl;
    std::cout << "transfer-encoding: " << this->transfer_encoding << std::endl;
    std::cout << "host: " << this->_host << std::endl;
    std::cout << blue << "-----------------Headers--------------------- " << def << std::endl;
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    if (this->_content_length > 0)
    {
        std::cout << blue << "-----------------Body--------------------- " << def << std::endl;
        std::cout << "bodyFileName: " << this->bodyFileName << std::endl;
    }
    std::cout << red << "--------------- End Request ----------------- " << def << std::endl;
}

std::string to_Lower_case(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

Request::Request(char *buffer, size_t bytes, int fd) : _method(""), _path(""), _version(""), _host(""),  _connection(""), _accept(""), _accept_encoding(""),  _content_type(""), _content_length(0), _headers(std::map<std::string, std::string>()), bodyFileName(""), client_fd(fd), _fdBodyFile(-1),  _is_complete(false), requestStatus(-1), status_message(""), bodyBytesWritten(0)
{
    std::stringstream ss((std::string(buffer)));
    std::string line;
    int         offset = 0;
    bool        is_first = true;

    while (std::getline(ss, line))
    {
        std::cout << B_blue << line << B_def << std::endl;
        offset += line.size() + 1;
        if (is_first)
        {
            std::vector<std::string> tmp(split(line, ' '));
            if (tmp.size() != 3)
            {
                badRequest();
                throw std::runtime_error("invalid request");
            }
            
            this->_method = (tmp[0]);
            this->_path = tmp[1];
            
            if (strncmp(tmp[2].c_str(), "HTTP/1.1", strlen("HTTP/1.1") ) == 0)
                this->_version = (tmp[2].substr(0, tmp[2].find_first_of("\r\n")));
            else 
                throw std::runtime_error("invalid version");
            
            is_first = false;
        }
        else if (!is_first)
        {
            if (line == "\r")
                break;
            std::vector<std::string> tmp = split(line, ':');
			std::pair<std::string, std::string> header;

			if (line.find(':') != (size_t)-1)
            {
				header.first = line.substr(0, line.find(':'));
				header.second = line.substr(line.find(':') + 2, line.length());
                //
                if (header.first == "Host")
                    this->_host = header.second;
                else if (header.first == "Connection")
                    this->_connection = header.second;
                else if (header.first == "Accept")
                    this->_accept = header.second;
                else if (header.first == "Accept-Encoding")
                    this->_accept_encoding = header.second;
                else if (header.first == "Accept-Language")
                    this->_accept_language = header.second;
                else if (header.first == "Content-Length")
                    this->_content_length = std::stoi(header.second);
                else if (header.first == "Content-Type")
                    this->_content_type = header.second;
                else if (header.first == "Transfer-Encoding")
                    this->transfer_encoding = header.second;
                else if (header.first == "Referer")
                    this->_headers.insert(std::pair<std::string, std::string>( to_Lower_case(header.first) , header.second));
			}
            else 
            {
                this->_isGoodRequest = this->is_formated = false;
                this->status_message =  "400 Bad Request\r\n";
                this->_path = "./errorsPages/400.html"; 
                std::cout << "bad request" << std::endl;
            }
            // std::cout << B_green <<"--->" << header.first << "/" << header.second << B_def << std::endl;
        }
    }

    if (_content_length == 0)
        _is_complete = true;
    else
    {
        bodyFileName = "./upload/file_" + std::to_string(this->_content_length) + "_" + std::to_string(rand() % 100);

        std::string s = this->_content_type;
        std::string delimiter = "\r";
        std::string token = s.substr(0, s.find(delimiter));
        token = token.substr(0, token.find("\n"));
        this->_content_type = token;
        if (this->_content_type == " image/jpeg")
            this->bodyFileName += ".jpeg";
        else if (this->_content_type == " image/png")
            this->bodyFileName += ".png";
        else if (this->_content_type == " text/html")
            this->bodyFileName += ".html";
        else if (this->_content_type == " text/css")
            this->bodyFileName += ".css";
        else if (this->_content_type == " text/javascript")
            this->bodyFileName += ".js";
        else if (this->_content_type == " text/plain")
            this->bodyFileName += ".txt";
        else if (this->_content_type == " video/mp4")
            this->bodyFileName += ".mp4";
        else if (this->_content_type == " application/pdf")
            this->bodyFileName += ".pdf";
        else if (this->_content_type == " application/x-www-form-urlencoded")
            this->bodyFileName += ".txt";
        else if (this->_content_type == " application/json")
            this->bodyFileName += ".json";
        else if (this->_content_type == " application/xml")
            this->bodyFileName += ".xml";
        else if (this->_content_type == " application/octet-stream")
            this->bodyFileName += ".bin";
        else if (this->_content_type == " application/zip")
            this->bodyFileName += ".zip";
        else
            this->bodyFileName += ".unknown";

        // std::cout << B_red << "bodyFileName: " << this->bodyFileName << B_def << std::endl;
        this->fill_body(buffer + offset, bytes - offset);
    }
}

void Request::fill_body(char *buffer, size_t bytes)
{
    int fd = open(this->bodyFileName.c_str(), O_RDWR | O_CREAT | (this->bodyBytesWritten == 0 ? O_TRUNC : O_APPEND), 0644);
    ssize_t written = write(fd, buffer, bytes);
    if (written != -1) {
        this->bodyBytesWritten += written;
    }
    close(fd);
    // std::cout << blue << "reading request body " << bytes  << B_def << std::endl;
    if (this->_content_length <= getFileSize(this->bodyFileName.c_str()))
    {
        // std::cout << B_green << "request body file : " << getFileSize(this->bodyFileName.c_str()) << B_def << std::endl;
        this->_is_complete = true;
    }
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}

const struct sockaddr_in &Request::getRefClientAddr() const {
	return _client_addr;
}