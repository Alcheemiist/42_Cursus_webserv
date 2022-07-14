#pragma once

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../elements.hpp"
#include <map>

class Request
{
private:
    std::string _method;
    std::string _path;
    std::string _version;
    std::pair<std::string, int> _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _content_type;
    std::map<std::string, std::string> _headers;
    size_t _content_length;
    long bytes;
    bool _is_complete;
    std::string _body;
    std::string _accept_language;
    int request_num;
    int requestStatus;

public:
    Request() : _method(""), _path(""), _version(""), _host("", 0),
                _connection(""), _accept(""), _accept_encoding(""),
                _content_type(""), _headers(std::map<std::string, std::string>()),
                _content_length(0), bytes(0), _is_complete(false), _body(""),
                _accept_language(""), request_num(0), requestStatus(1){};
    Request(char *buffer, size_t bytes);
    ~Request(){};
    // OVERLOAD OPERATORS ---------------------------------------
    Request &operator++()
    {
        ++request_num;
        return *this;
    }
    // SETTERS --------------------------------------------------
    void setbytes(long bytes) { this->bytes = bytes; }
    // GETTERS --------------------------------------------------
    long getbytes() { return bytes; }
    int getRequestNum() const { return request_num; };
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    bool getIsComplete() const { return _is_complete; };
    std::string getConnection() const { return _connection; };
    int getRequestStatus() const { return requestStatus; };

    // METHODS --------------------------------------------------
    bool isGoodrequest() { return (true); };
    void parse(char *buffer);
    void fill_body(char *buffer, size_t bytes);
    char *readFile(const char *fileName);
    void show();
};

char *readFile(const char *fileName);
long readRequest(int new_socket, Request *request);
void checkRequest(Request *request);

#endif