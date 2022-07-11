#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

class Response
{
private:
    std::string version;
    std::string status;
    std::string header;
    std::string body;
    std::string response;
    std::string responseStatus;
    size_t body_length;

public:
    Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0){};
    void setVersion(std::string version) { this->version = version; };
    void setStatus(std::string status) { this->status = status; };
    void setHeader(char *header) { this->header = header; };
    void setBody(char *body) { this->body = body; };
    void setResponseStatus(char *status) { this->responseStatus = status; };
    void setResponseHeader() { this->header = (char *)this->version.c_str(); };
    size_t size() const { return body_length; };
    std::string getResponse() const
    {
        std::string res;
        res.append(version);
        res.append(status);
        res.append("\r\n\r\n");
        res.append(body);
        return res;
    };
    std::string getBody() { return body; };
};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse();
void PUTresponse();
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
void response(int new_socket, Request *request, parse_config *config);

char *readFile(const char *fileName);

#endif