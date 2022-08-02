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
        Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0), contentType(""),
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
        std::string getHeader() 
        {
            /* Here to formed other headers */
            // status line
            std::string res;
            res.append(version);
            res.append(status);
            // headers
            res.append("Content-Type: ");
            res.append(this->contentType);
            res.append("\r\n");

            size_t tt = getFileSize(body_file_path.c_str());
            std::cout << tt << " {" <<  body_file_path << std::endl;
            
            // if (tt > 0)
            // {
            //     res.append("Content-Length: ");
            //     res.append(std::to_string(tt));
            //     res.append("\r\n");
            // }
            // else
            // {
            //     res.append("Content-Length: ");
            //     res.append(std::to_string(0));
            //     res.append("\r\n");
            // }
            res.append("server: alchemist\r\n");
            res.append("location: wonderland");
            // CRLF
            res.append("\r\n\r\n");
            setHeader(res);

            return res;
        };
        std::string getBody() { return body; };
        void    setbody_file_size(int size) { this->body_file_size = size; };
        int     getbody_file_size() { return this->body_file_size; };
        void    set_finish(bool i) { this->is_complete = i; };
        void    setContentType(char *path);
        bool    get_finish() { return this->is_complete; };
        std::vector<char> get_buffer() {
            std::vector <char> buffer;

            int fd = open(this->body_file_path.c_str(), O_RDONLY);
            if (fd < 0)
            {
                std::cout << "open file error";
            }
            int size = lseek(fd, maxBufferLenght, SEEK_SET);
            if (size < 0)
                std::cout << "lseek error";
            char *buf = (char *)malloc((1024 * 16) + 1);
            int read_size = read(fd, buf, (1024 * 16));
            if (read_size < 0)
                std::cout << "read error";
            maxBufferLenght += read_size;
            for (int i = 0; i < read_size; i++)
                buffer.push_back(buf[i]);
            free(buf);
            close(fd);
            if (maxBufferLenght >= getFileSize(body_file_path.c_str()))
                this->is_complete = true;
            else 
                this->is_complete = false;
            std::cout << red << "- Read_size  : " << read_size << " maxBuffer : " << maxBufferLenght << " file path size : " << getFileSize(body_file_path.c_str()) << def << std::endl;
            return buffer;
        };
        //
        void set_maxBufferLenght(size_t size) { this->maxBufferLenght = size; };
        size_t get_maxBufferLenght() { return this->maxBufferLenght; };
        void set_requestFuckedUp(bool i) { this->requestFuckedUp = i; };
        bool get_requestFuckedUp() { return this->requestFuckedUp; };
        //
        void setStatus(Request *request, Server server);
        void show() { std::cout << red << "Header : SOF-{" << def << this->header << red << "}-EOF" << def << std::endl;  };
        void set_redirection(std::string url)
        {
            this->redirection = url;
        };
        std::string get_redirection()
        {
            return this->redirection;
        };
        void set_location(std::string url)
        {
            this->location = url;
        };
        std::string get_location()
        {
            return this->location;
        };

};

void GETresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void POSTresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void PUTresponse();
void DELETEresponse(Request *request, Response *response, ParseConfig *config, int server_index);
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
Response response(int new_socket, Request *request, ParseConfig *config, int fd_server);
char *readFile(const char *fileName);


#endif


// #pragma once

// #ifndef RESPONSE_HPP
// #define RESPONSE_HPP

// #include "../elements.hpp"
// #include <vector>
// #define MAX_URL_LENGTH 2048


// class Response
// {
//     private:
//         std::string version;
//         std::string status;
//         std::string header;
//         std::string body;
//         std::string response;
//         std::string responseStatus;
//         size_t      body_length;
//         std::string contentType;
//         std::vector<std::string> status_vector;
//         std::string location_url;
//         std::string redirection_url;

//     public:
//         Response();
//         void setVersion(std::string version);
//         void setStatus(Request *request, Server server);
//         void setStatus(std::string status);
//     	bool url_is_formated(std::string url);
//         void setHeader(char *header);
//         void setBody(char *body);
//         void setResponseStatus(char *status);
//         void setResponseHeader();
//         size_t size() const;
//         std::string getResponse() const;
//         std::string getBody() { return body; };
//         void setContentType(char *path);
//         void set_redirection_url(std::string url);
//         void set_location_url(std::string url);
//         std::string get_redirection_url();
//         std::string get_location_url();
// };

// void GETresponse(Request *request, Response *response, Config *config);
// void POSTresponse(Request *request, Response *response, ParseConfig *config, int index_server);
// void PUTresponse(Request *request, Response *response, ParseConfig *config, int index_server);
// void DELETEresponse();
// void HEADresponse();
// void ERRORresponse(Request *request, Response *response);
// void response(int new_socket, Request *request, ParseConfig *config,
//         int fd_server);

// char *readFile(const char *fileName);

// #endif