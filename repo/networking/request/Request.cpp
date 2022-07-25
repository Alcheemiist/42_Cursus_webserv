
#include "../elements.hpp"
#include "../../webserve.hpp"

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

void Request::fill_body(char *buffer, size_t bytes)
{
    int fd = open(this->bodyFileName.c_str(), O_RDWR | O_CREAT | O_APPEND, 0666);
    write(fd, buffer, bytes);
    close(fd);
    if (this->_content_length <= getFileSize(this->bodyFileName.c_str()))
        this->_is_complete = true;
}

char *Request::readFile(const char *fileName)
{
    FILE *pFile;
    char buffer[100];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);

    pFile = fopen(fileName, "r");
    if (pFile == NULL)
        throw std::runtime_error("File not found");
    else
    {
        int i = 0;
        while (!feof(pFile))
        {
            if (fgets(buffer, 100, pFile) == NULL)
                break;
            strcpy(return_buffer + i, buffer);
            i += strlen(buffer);
        }
        fclose(pFile);
    }
    return return_buffer;
}

Request::Request(char *buffer, size_t bytes, int fd) : client_fd(fd)
{
    std::stringstream ss((std::string(buffer)));
    std::string line;
    int offset = 0;
    bool is_first = true;

    _content_length = 0;
    _method = "";
    bodyFileName = "";
    _is_complete = false;

    while (std::getline(ss, line))
    {
        std::cout << B_blue << line << B_def << std::endl;
        offset += line.size() + 1;
        if (is_first)
        {
            std::vector<std::string> tmp(split(line, ' '));
            if (tmp.size() != 3)
                throw std::runtime_error("invalid request");
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
                this->_host = std::make_pair(tmp[1], std::stoi(tmp[2]));
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
            else
                this->_headers.insert(std::pair<std::string, std::string>(tmp[0], tmp[1]));
        }
    }
    if (_content_length == 0)
        _is_complete = true;
    else
    {
        bodyFileName = ".tmp/file_" + std::to_string(this->_content_length) + "_" + std::to_string(rand() % 100);

        std::string s = this->_content_type;
        std::string delimiter = "\r";
        std::string token = s.substr(0, s.find(delimiter));
        token = token.substr(0, token.find("\n"));
        std::cout << B_red << " content_type :{" << token << "}" << std::endl;

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
        else
            this->bodyFileName += ".unknown";

        this->fill_body(buffer + offset, bytes - offset);
    }
}

void Request::show()
{
    std::cout << red << "--------------- Request ----------------- " << def << std::endl;
    std::cout << "method: " << this->_method << std::endl;
    std::cout << "path: " << this->_path << std::endl;
    std::cout << "version: " << this->_version << std::endl;
    std::cout << "host: " << this->_host.first << ":" << this->_host.second << std::endl;
    std::cout << "connection: " << this->_connection << std::endl;
    std::cout << "accept: " << this->_accept << std::endl;
    std::cout << "accept-encoding: " << this->_accept_encoding << std::endl;
    std::cout << "accept-language: " << this->_accept_language << std::endl;
    std::cout << "content-length: " << this->_content_length << std::endl;
    std::cout << "content-type: " << this->_content_type << std::endl;

    std::cout << blue << "-----------------Headers--------------------- " << def << std::endl;
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    if (this->_content_length)
        std::cout << green << "----------------- REQUEST BODY --- => " << getFileSize(this->bodyFileName.c_str()) << green << " ------------------------ " << def << std::endl;
    std::cout << red << "--------------- End Request ----------------- " << def << std::endl;
}

void checkRequest(Request *request)
{
    (void)request;
}