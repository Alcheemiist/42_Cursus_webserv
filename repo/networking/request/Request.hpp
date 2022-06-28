#pragma once 


#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../elements.hpp"

class Request
{
private:
    std::string _type;
    char buffer[30000];
    long bytes;
    int request_num;
    int requestStatus;
    std::string _method;
    std::string _path;
    std::string _version;
    std::string _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _accept_language;
    std::string _port;
    std::string _body;

public:
    Request()
    {
        request_num = 0;
        _type = "GET";
        bytes = 0;
        requestStatus = 0;
        _method = "GET";
        _path = "./www/index.html";
        _version = "HTTP/1.1";
        _host = "localhost";
        _connection = "keep-alive";
        _accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8";
        _accept_encoding = "gzip, deflate, br";
        _accept_language = "en-US,en;q=0.9";
        _port = "80";
        _body = "";
    };

    // OVERLOAD OPERATORS --------------------------------------------------
    Request &operator++() { ++request_num;  return *this; }

    // SETTERS --------------------------------------------------
    void setbytes(long bytes) { this->bytes = bytes; }
    void setbuffer(char *buffer) { std::strcpy(this->buffer, buffer); }

    // GETTERS --------------------------------------------------
    long getbytes() { return bytes; }
    char *getbuffer() { return buffer; }
    std::string getType() const { return _type; };
    int getRequestNum() const { return request_num; };
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    std::string getHost() const { return _host; };
    std::string getPort() const { return _port; };
    std::string getConnection() const { return _connection; };

    // METHODS --------------------------------------------------
    int isGoodrequest() { return (requestStatus); };


};

char *readFile(const char * fileName);
long readRequest(int new_socket, Request *request);
void checkRequest(Request *request);

#endif 