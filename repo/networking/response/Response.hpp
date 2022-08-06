#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

static int  _BUFFER_SIZE = 1024 * 16;         //

class Request;

size_t getFileSize(const char *fileName);

class Request;

class Response
{
    private:
        std::string version;
        std::string status;
        std::string header;
        std::string body;
        std::string response;
        std::string contentType;
        //
        size_t      body_file_size;
        bool        is_complete;
        std::string body_file_path;
        size_t      maxBufferLenght;
        //
        std::vector<std::string> status_vector;
        std::string requested_path;
        std::string redirection_path;
        std::string index;
        // 
        bool is_cgi;
		bool is_autoindex;

    public:
        Response() : version("HTTP/1.1"), status(""), header(""), body(""), response(""), contentType(""),
        body_file_size(0),  is_complete(false), body_file_path(""), maxBufferLenght(0), is_autoindex(false) {};

        void setVersion(std::string version) { this->version = version; };
        void  setStatus(std::string status) { this->status = status; };
        void    setHeader(char *_header) { this->header = _header; };
        void setHeader(std::string header) { this->header = header; };
        void setBody(char *body)  {  this->body = body;   };
        void setBody(std::vector<char> _body)
        {
            for (std::vector<char>::iterator it = _body.begin(); it != _body.end(); ++it)
                this->body += *it;
        };
        void setpath (std::string path){ this->body_file_path = path; }
        std::string getpath (){ return this->body_file_path; }
        void setResponseHeader() { this->header = this->version.c_str(); std::cout << "- Set Version : " << this->version << std::endl;};
        std::string get_header() { return this->header; };
        std::string getHeader();
        std::string getBody() { return body; };
        void    setbody_file_size(int size) { this->body_file_size = size; };
		void	set_autoindex(bool ai) { this->is_autoindex = ai; };
        int     getbody_file_size() { return this->body_file_size; };
        void    set_finish(bool i) { this->is_complete = i; };
        void    setContentType(std::string type);
        bool    get_finish() { return this->is_complete; };
        std::vector<char> get_buffer();
        //
        void set_maxBufferLenght(size_t size) { this->maxBufferLenght = size; };
        size_t get_maxBufferLenght() { return this->maxBufferLenght; };

        //
        std::string setStatus(Request *request, Server server);
        void show() { std::cout << red << "Header : SOF-{" << def << this->header << red << "}-EOF" << def << std::endl;  };
        std::string get_location() {  return this->requested_path; };
        std::string get_redirection()  { return this->redirection_path; };
        void init_location(std::string url, Server server);
        void set_location(std::string url, Server server);
        void init_redirection(std::string url, Server server, std::string &ref);
        void set_redirection(std::string url) { this->redirection_path = url; };
        std::string get_version(){ return this->version; };
        std::string get_status(){ return this->status; };
        std::string get_content_type(){ return this->contentType; };
        void set_index(std::string str) { index = str; };
        std::string get_index() {return (index); }
        std::string get_index(std::string url, Server server);
};

std::string ERRORresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string GETresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string DELETEresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string POSTresponse(Request *request, Response *response, ParseConfig *config, int server_index);
Response response( Request *request, ParseConfig *config, int fd_server);
std::string URLgetFileName(std::string url);



#endif