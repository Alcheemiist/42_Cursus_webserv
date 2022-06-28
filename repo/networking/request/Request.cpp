
#include "../elements.hpp"
#include "../../webserve.hpp"
#include <sstream>
#include <fcntl.h>
#include <vector>
#include <sys/stat.h>

// split string by delimiter
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

// REQUEST --------------------------------------------------

// calculate file size
size_t getFileSize(const char *fileName)
{
    struct stat st;
    stat(fileName, &st);
    return st.st_size;
}

void Request::fill_body(char *buffer, size_t bytes)
{
    std::cout << "content length: " << _content_length << std::endl;
    int fd = open(this->_body.c_str(), O_RDWR | O_CREAT | O_APPEND, 0666);
    write(fd, buffer, bytes);
    close(fd);
    if(this->_content_length <= getFileSize(this->_body.c_str()))
    {
        // std::cout << "body size: " << getFileSize(this->_body.c_str()) << std::endl;
        this->_is_complete = true;
    }
}

Request::Request(char *buffer, size_t bytes)
{
    std::cout << "bytes" << bytes << std::endl;
    std::string request(buffer);
    std::stringstream ss(request);
    std::string line;
    bool is_first = true;
    int offset = 0;
    _content_length = 0;
    _method = "";
    _port = "";
    _body = "body.png";
    _is_complete = false;

    while (std::getline(ss, line))
    {
        offset += line.size() + 1;
        if (is_first)
        {
            std::vector<std::string> tmp = split(line, ' ');
            this->_method = tmp[0];
            this->_path = tmp[1];
            this->_version = tmp[2];
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
            else if (tmp[0] == "Port")
                this->_port = tmp[1];
            else if (tmp[0] == "Content-Length")
                this->_content_length = std::stoi(tmp[1]);
            else
                this->_headers.insert(std::pair<std::string, std::string>(tmp[0], tmp[1]));
        }
    }
    if (_content_length == 0)
        _is_complete = true;
    else
        this->fill_body(buffer + offset, bytes - offset);
}

// long readRequest(int new_socket, Request *request)
// {
//     char *buffer = (char *)malloc(sizeof(char) * 30000000);

//     Color::Modifier red(Color::FG_RED);
//     Color::Modifier def(Color::FG_DEFAULT);
//     Color::Modifier blue(Color::FG_BLUE);
//     Color::Modifier green(Color::FG_GREEN);
//     Color::Modifier B_red(Color::BG_RED);

//     // Read the request from the client
//     request->setbytes( recv(new_socket, buffer, 3000000 , 0) );
//     // int fd = open("request.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
//     // write(fd, buffer, request->getbytes());
//     // close(fd);
//     // Print the request
//     request->parse(buffer);

//     std::cout << red << "\n----(Request*" << request->getRequestNum() << ")------- (N Bytes: " << request->getbytes() << ")------------ "<< def << std::endl;
//     std::cout << buffer << std::endl;
//     std::cout << red << "-------------------------------------------------" << def << std::endl;
    
//     // Parse the request & fill the request object

//     // Print the request class attributes

//     // Free local buffers
//     free(buffer);

//     return request->getbytes();
// }

void checkRequest(Request *request)
{
    (void)request;
}