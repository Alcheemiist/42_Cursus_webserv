#pragma once

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../elements.hpp"
#include <map>

#define ERROR_VALUE -99

class Request
{
private:
    //
    std::string _method;
    std::string _path;
    std::string _version;
    std::pair<std::string, int> _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _content_type;
    size_t _content_length;
    std::string _accept_language;
    //
    std::map<std::string, std::string> _headers;
    //
    std::string bodyFileName;
    int client_fd;
    int _fdBodyFile;
    long bytes;
    //
    bool _isGoodRequest;
    bool _is_complete;
    int requestStatus;
    std::string status_message;
    //
    ssize_t bodyFileSize;

public:
    Request() : _method("ALCHEMIST"), _path("ALCHEMIST"), _version("ALCHEMIST"), _host("ALCHEMIST", ERROR_VALUE),
                _connection("ALCHEMIST"), _accept("ALCHEMIST"), _accept_encoding("ALCHEMIST"),
                _content_type("ALCHEMIST"), _content_length(ERROR_VALUE), _headers(std::map<std::string, std::string>()),
                //
                bodyFileName(""), client_fd(ERROR_VALUE), _fdBodyFile(ERROR_VALUE), bytes(ERROR_VALUE),
                _is_complete(false), requestStatus(ERROR_VALUE), status_message("ALCHEMIST"){};

    Request(char *buffer, size_t bytes, int fd);
    ~Request(){};
    //
    void setbytes(long bytes) { this->bytes = bytes; }
    //
    long getbytes() { return bytes; }
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    bool getIsComplete() const { return _is_complete; };
    std::string getConnection() const { return _connection; };
    int getRequestStatus() const { return requestStatus; };
    //
    void parse(char *buffer);
    void fill_body(char *buffer, size_t bytes);
    char *readFile(const char *fileName);
    void show();
    //
    void checkRequest();
    bool isGoodrequest() { return (true); };
};
//
char *readFile(const char *fileName);
long readRequest(int new_socket, Request *request);

#endif