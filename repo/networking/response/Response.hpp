#pragma once 

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

class Response
{
private:
    char hello[10000];
    // int response_num;
    // int responseStatus;
    std::string version;
    std::string status;
    char *body;
    char *response_buffer;
public:
    Response(): version("HTTP/1.1"), status("200 OK")
    {
        // response_num = 0;
        // responseStatus = 0;
        body = NULL;
        response_buffer = (char *)malloc(sizeof(char) * 30000000);
    };

    const char *getHello() const { return hello; };

    void generateResponse(Request *request)
    {
        char res[100000];

        body = readFile(request->getPath().c_str());
        
        strcpy(res, version.c_str());
        strcpy( res + strlen(version.c_str())  ,status.c_str());
        strcpy( res + strlen(res)  ,readFile(request->getPath().c_str()));
        strcpy(response_buffer, res);

        // std::cout << "**************** Response *****************\n";
        // std::cout << res << std::endl;
        // std::cout << "**************** Response *****************\n";
    }
    size_t getSize() const { return strlen(response_buffer); };
    char *getResponse() const { return response_buffer; };
    void setBody(char *_body) { strcpy(body, _body); };
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