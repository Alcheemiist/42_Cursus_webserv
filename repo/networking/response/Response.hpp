#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

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
        size_t body_length;
        std::string contentType;
        //
        size_t      body_file_size;
        bool        is_complete;
        std::string body_file_path;
        size_t      maxBufferLenght;

    public:
        Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0), contentType(""), body_file_size(0),
        is_complete(false), body_file_path(""), maxBufferLenght(0) {};
        
        void setVersion(std::string version)
        {
            this->version = version;
        };
        void setStatus(std::string status)
        {
            this->status = status;
        };
        void setHeader(char *header)
        {
            this->header = header;
        };
        void setBody(char *body)
        {
            this->body = body;
        };
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
        std::string getHeader() 
        {
            // status line
            std::string res;
            res.append(version);
            res.append(status);
            // headers
            res.append("Content-Type: ");
            res.append(this->contentType);
            res.append("\r\n");
            if (body_file_size > 0)
            {
                size_t tt = getFileSize(body_file_path.c_str());
                res.append("Content-Length: ");
                res.append(std::to_string(tt));
                res.append("\r\n");
            }
            else
            {
                res.append("Content-Length: ");
                res.append(std::to_string(0));
                res.append("\r\n");
            }
            res.append("server: alchemist\r\n");
            res.append("location: wonderland");
            // CRLF
            res.append("\r\n\r\n");
            return res;
        };
        std::string getBody() { return body; };
        void    setbody_file_size(int size) { this->body_file_size = size; };
        int     getbody_file_size() { return this->body_file_size; };
        void    set_finish(bool i) { this->is_complete = i; };
        void    setContentType(char *path)
        {
            std::string s(path);
            std::string s1 = s.substr(s.find_last_of(".") + 1);

            if (s1 == "html")
                this->contentType = "text/html";
            else if (s1 == "css")
                this->contentType = "text/css";
            else if (s1 == "js")
                this->contentType = "application/javascript";
            else if (s1 == "jpg")
                this->contentType = "image/jpeg";
            else if (s1 == "png")
                this->contentType = "image/png";
            else if (s1 == "gif")
                this->contentType = "image/gif";
            else if (s1 == "ico")
                this->contentType = "image/x-icon";
            else if (s1 == "svg")
                this->contentType = "image/svg+xml";
            else if (s1 == "mp3")
                this->contentType = "audio/mpeg";
            else if (s1 == "mp4")
                this->contentType = "video/mp4";
            else if (s1 == "ogg")
                this->contentType = "audio/ogg";
            else if (s1 == "ogv")
                this->contentType = "video/ogg";
            else if (s1 == "wav")
                this->contentType = "audio/wav";
            else if (s1 == "webm")
                this->contentType = "video/webm";
            else if (s1 == "txt")
                this->contentType = "text/plain";

            std::cout << red << "- Set Content-Type : " << s << green << " " << s1 << " " << this->contentType << def << std::endl;
        };
        bool    get_finish() { return this->is_complete; };
        std::vector<char > get_buffer()
        {
            std::vector <char > buffer;

            int fd = open(this->body_file_path.c_str(), O_RDONLY);
            if (fd < 0)
            {
                
                std::cout << "open file error";
            }
            int size = lseek(fd, maxBufferLenght, SEEK_SET);
            if (size < 0)
                std::cout << "lseek error";
            char *buf = (char *)malloc(1025);
            int read_size = read(fd, buf, 1024);
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

        void set_maxBufferLenght(size_t size) { this->maxBufferLenght = size; };
        size_t get_maxBufferLenght() { return this->maxBufferLenght; };
};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse();
void PUTresponse();
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
Response response(int new_socket, Request *request, ParseConfig *config, int fd_server);
char *readFile(const char *fileName);

#endif