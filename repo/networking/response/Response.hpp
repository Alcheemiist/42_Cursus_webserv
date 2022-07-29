#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"
#include <vector>


class Response
{
private:
    std::string version;
    std::string status;
    std::string header;
    std::string body;
    std::string response;
    std::string responseStatus;
    size_t      body_length;
    std::string contentType;
    std::vector<std::string> status_vector;
    std::string location;

public:
    Response();
    void setVersion(std::string version);
    void setStatus(Request *request, parse_config *config);
	bool url_parser(std::string url);
    void setHeader(char *header);
    void setBody(char *body);
    void setResponseStatus(char *status);
    void setResponseHeader();
    size_t size() const;
    std::string getResponse() const;
    std::string getBody() { return body; };
    void setContentType(char *path);
};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse();
void PUTresponse();
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
void response(int new_socket, Request *request, parse_config *config,
        int fd_server);

char *readFile(const char *fileName);

#endif