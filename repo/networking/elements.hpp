#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "../webserve.hpp"

char hello[1000] = "HTTP/1.1 200 OK\nAlchemist: is here\n\n<div class=\"hilite-title text-center text-uppercase\">ALCHEMIST</div>";

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
        _path = "/";
        _version = "HTTP/1.1";
        _host = "localhost";
        _connection = "keep-alive";
        _accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8";
        _accept_encoding = "gzip, deflate, br";
        _accept_language = "en-US,en;q=0.9";
        _port = "80";
        _body = "";
    };

    Request &operator++()
    {
        ++request_num;
        return *this;
    }

    void setbytes(long bytes) { this->bytes = bytes; }
    long getbytes() { return bytes; }
    void setbuffer(char *buffer) { std::strcpy(this->buffer, buffer); }
    char *getbuffer() { return buffer; }
    std::string getType() const { return _type; };
    int getRequestNum() const { return request_num; };
    int isGoodrequest() { return (requestStatus); };
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    std::string getHost() const { return _host; };
    std::string getConnection() const { return _connection; };
};
class Response
{
private:
    char hello[10000];
    // int response_num;
    // int responseStatus;
    std::string version;
    std::string status;
    std::string body;

public:
    Response() { strcpy(hello, "HTTP/1.1 200 OK\nAlchemist: is here\n\n<div>ALCHEMIST</div>"); };
    const char *getHello() const { return hello; };
    size_t getSize() const { return strlen(hello); };
};

#endif