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
    std::string _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _content_type;
    size_t      _content_length;
    std::string _accept_language;
    std::map<std::string, std::string> _headers;
    std::string bodyFileName;
    int client_fd;
    int _fdBodyFile;
    bool _isGoodRequest;
    bool _is_complete;
    int requestStatus;
    std::string status_message;
    ssize_t bodyFileSize;

public:
    Request() : _method(""), _path(""), _version(""), _host(""), _connection(""), _accept(""), _accept_encoding(""),  _content_type(""), _content_length(-1), _headers(std::map<std::string, std::string>()),
                bodyFileName(""), client_fd(-1), _fdBodyFile(-1), _is_complete(false), requestStatus(-1), status_message(""){};
    Request(char *buffer, size_t bytes, int fd);
    ~Request(){};
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    bool        getIsComplete() const { return _is_complete; };
    std::string getConnection() const { return _connection; };
    int         getRequestStatus() const { return requestStatus; };
    const std::map<std::string, std::string> &getHeaders() const;
    void fill_body(char *buffer, size_t bytes);
    char *readFile(const char *fileName);
    void show();
    void checkRequest();
    void badRequest() { requestStatus = -1; status_message = "Bad Request";_isGoodRequest = false; };
    void goodRequest() { requestStatus = 1; status_message = "Good Request";_isGoodRequest = true; };
    bool isGoodrequest() { return (true); };
};
char *readFile(const char *fileName);
long readRequest(int new_socket, Request *request);
std::vector<std::string> split(const std::string &s, char delim);
size_t getFileSize(const char *fileName);
std::string to_Lower_case(std::string str);

#endif