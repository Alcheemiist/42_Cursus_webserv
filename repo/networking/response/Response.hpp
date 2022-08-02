#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"
#define MAX_URL_LENGTH 2048

size_t getFileSize(const char *fileName);

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
        //
        size_t      body_file_size;
        bool        is_complete;
        std::string body_file_path;
        size_t      maxBufferLenght;
        bool        requestFuckedUp;
        //
        std::vector<std::string> status_vector;
        std::string location;
        std::string redirection;

    public:
        Response() : version("HXXXX "), status("! 200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0), contentType(""),
        body_file_size(0),  is_complete(false), body_file_path(""), maxBufferLenght(0), requestFuckedUp(false) {};

        void setVersion(std::string version) { this->version = version; };
        void setStatus(std::string status) { this->status = status; };
        void setHeader(char *_header) { this->header = _header; };
        void setHeader(std::string header) { this->header = header; };
        void setBody(char *body)  {  this->body = body;   };
        void setBody(std::vector<char> _body)
        {
            for (std::vector<char>::iterator it = _body.begin(); it != _body.end(); ++it)
                this->body += *it;
        };
        void setpath (std::string path){ this->body_file_path = path; }
        std::string getpath (){ return this->body_file_path; }
        void setResponseStatus(char *_status) 
        {
            this->status = _status;
            std::cout << "- Set Status : " << this->status << std::endl;
        };
        void setResponseHeader() { this->header = this->version.c_str(); std::cout << "- Set Version : " << this->version << std::endl;};
        size_t size() const { return body_length; };
        std::string get_header() { return this->header; };
        std::string getHeader();
        std::string getBody() { return body; };
        void    setbody_file_size(int size) { this->body_file_size = size; };
        int     getbody_file_size() { return this->body_file_size; };
        void    set_finish(bool i) { this->is_complete = i; };
        void    setContentType(char *path);
        bool    get_finish() { return this->is_complete; };
        std::vector<char> get_buffer();
        //
        void set_maxBufferLenght(size_t size) { this->maxBufferLenght = size; };
        size_t get_maxBufferLenght() { return this->maxBufferLenght; };
        void set_requestFuckedUp(bool i) { this->requestFuckedUp = i; };
        bool get_requestFuckedUp() { return this->requestFuckedUp; };
        //
        void setStatus(Request *request, Server server);
        void show() { std::cout << red << "Header : SOF-{" << def << this->header << red << "}-EOF" << def << std::endl;  };
        void set_redirection(std::string url)  { this->redirection = url;  };
        std::string get_redirection() {  return this->redirection; };
        void set_location(std::string url) { this->location = url; };
        std::string get_location()  { return this->location;  };
};

void GETresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void POSTresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void PUTresponse();
void DELETEresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
Response response( Request *request, ParseConfig *config, int fd_server);
char *readFile(const char *fileName);

std::string URLgetFileName(std::string url);



#endif