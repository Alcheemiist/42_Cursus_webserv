#include "../elements.hpp"

Request::Request(char *buffer, size_t bytes, int fd) : _method(""), _path(""), _version(""), _host(""),  _connection(""), _accept(""), _accept_encoding(""),  _content_type(""), _content_length(0), _headers(std::map<std::string, std::string>()), bodyFileName(""), client_fd(fd), _fdBodyFile(-1),  _is_complete(false), requestStatus(-1), status_message("")
{
    std::stringstream ss((std::string(buffer)));
    std::string line;
    int         offset = 0;
    bool        is_first = true;

    while (std::getline(ss, line))
    {
        //std::cout << B_blue << line << B_def << std::endl;
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
            this->_version = (tmp[2]);
            is_first = false;
        }
        else if (!is_first)
        {
            if (line == "\r")
                break;
            std::vector<std::string> tmp = split(line, ':');
            if (tmp[0] == "Host")
                this->_host = tmp[1];
            else if (tmp[0] == "Connection")
                this->_connection = tmp[1];
            else if (tmp[0] == "Accept")
                this->_accept = tmp[1];
            else if (tmp[0] == "Accept-Encoding")
                this->_accept_encoding = tmp[1];
            else if (tmp[0] == "Accept-Language")
                this->_accept_language = tmp[1];
            else if (tmp[0] == "Content-Length")
                this->_content_length = std::stoi(tmp[1]);
            else if (tmp[0] == "Content-Type")
                this->_content_type = tmp[1];
            else if (tmp[0] == "referer")
                this->_headers.insert(std::pair<std::string, std::string>(tmp[0], tmp[1] + tmp[2] ));
            else
                this->_headers.insert(std::pair<std::string, std::string>(tmp[0], tmp[1]));
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

        std::cout << B_red << "bodyFileName: " << this->bodyFileName << B_def << std::endl;
        this->fill_body(buffer + offset, bytes - offset);
    }
}

void Request::fill_body(char *buffer, size_t bytes)
{
    int fd = open(this->bodyFileName.c_str(), O_RDWR | O_CREAT | O_APPEND, 0666);
    write(fd, buffer, bytes);
    close(fd);
    // std::cout << blue << "reading request body " << bytes  << B_def << std::endl;
    if (this->_content_length <= getFileSize(this->bodyFileName.c_str()))
    {
        std::cout << B_green << "request body file : " << getFileSize(this->bodyFileName.c_str()) << B_def << std::endl;
        this->_is_complete = true;
    }
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}
