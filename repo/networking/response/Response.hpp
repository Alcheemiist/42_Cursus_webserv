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
    std::string contentType;

public:
    Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0), contentType(""){};
    void setVersion(std::string version)
    {
        this->version = version;
        std::cout << "- Set Version : " << this->version << std::endl;
    };
    void setStatus(std::string status)
    {
        this->status = status;
        std::cout << "- Set Status : " << this->status << std::endl;
    };
    void setHeader(char *header)
    {
        this->header = header;
        std::cout << "- Set header : " << this->header << std::endl;
    };
    void setBody(char *body)
    {
        this->body = body;
        std::cout << "- Set body of size = " << this->body.size() << std::endl;
    };
    void setResponseStatus(char *status)
    {
        this->responseStatus = status;
        std::cout << "- Set Status : " << this->responseStatus << std::endl;
    };

    void setResponseHeader()
    {
        this->header = this->version.c_str();
        std::cout << "- Set Version : " << this->version << std::endl;
    };
    size_t size() const { return body_length; };
    std::string getResponse() const
    {
        std::string res;
        res.append(version);
        res.append(status);
        // res.append("Content-Type: ");
        // res.append(this->contentType);
        // res.append("\r\n");
        res.append("server: alchemist\r\n");
        res.append("location: wonderland");
        res.append("\r\n\r\n");
        res.append(body);
        if (res.size() < 1000)
            std::cout << green << "{" << res << "}" << def << std::endl;
        else
            std::cout << green << "BUFFER SEND SIZE " << res.size() << def << std::endl;
        return res;
    };
    std::string getBody() { return body; };

    void setContentType(char *path)
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

        std::cout << red << "- Set Content-Type : " << s << green << " " << s1 << def << std::endl;
    };
};

void GETresponse(Request *request, Response *response, Config *config);
void POSTresponse();
void PUTresponse();
void DELETEresponse();
void HEADresponse();
void ERRORresponse(Request *request, Response *response);
void response(int new_socket, Request *request, parse_config *config, int fd_server);

char *readFile(const char *fileName);

#endif