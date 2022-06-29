#pragma once 

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

class Response
{
private:

    std::string version;
    std::string status;

    char *header;
    char *body;
    char *response;

    char * responseStatus;
    size_t body_length;

public:
    
    Response(): version(""),
                status(""),
                header(nullptr),
                body(nullptr),
                response(nullptr),
                responseStatus(nullptr),
                body_length(0) 
    {};
    

    size_t size() const { return strlen(response); };
    void setVersion(std::string version) { this->version = version; };
    void setStatus(std::string status) { this->status = status; };

    void setHeader(char *header) { this->header = header; };
    void setBody(char *body) { this->body = body; };
    void setResponseStatus( char * status) { this->responseStatus = status; };
    void setResponseHeader() { this->header = (char *)this->version.c_str(); };


    char *getResponse() const 
    { 
        strcpy(this->response, this->version.c_str());
        strcpy(this->response + strlen(this->version.c_str()), this->responseStatus);
        strcpy(this->response + strlen(this->version.c_str()) + strlen(this->responseStatus), this->body);
        body_length = (const size_t)strlen((const char *)response);
        return response;
    };
    char *getBody() { return body; };
};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse();
void PUTresponse();
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
void response(int new_socket, Request request, Config *config);

char *readFile(const char * fileName);


#endif