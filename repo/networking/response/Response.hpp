#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"
#include <vector>
#define MAX_URL_LENGTH 2048


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
        std::string location_url;
        std::string redirection_url;

    public:
        Response();
        void setVersion(std::string version);
        void setStatus(Request *request, Server server);
        void setStatus(std::string status);
    	bool url_is_formated(std::string url);
        void setHeader(char *header);
        void setBody(char *body);
        void setResponseStatus(char *status);
        void setResponseHeader();
        size_t size() const;
        std::string getResponse() const;
        std::string getBody() { return body; };
        void setContentType(char *path);
        void set_redirection_url(std::string url);
        void set_location_url(std::string url);
        std::string get_redirection_url();
        std::string get_location_url();

};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse(Request *request, Response *response, ParseConfig *config, int index_server);
void PUTresponse(Request *request, Response *response, ParseConfig *config, int index_server);
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
void response(int new_socket, Request *request, ParseConfig *config,
        int fd_server);

char *readFile(const char *fileName);

#endif