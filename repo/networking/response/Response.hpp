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

public:
    Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""), body(""), response(""), responseStatus(""), body_length(0), contentType("")
    {

    };

    void setVersion(std::string version)
    {
        this->version = version;
        std::cout << "- Set Version : " << this->version << std::endl;
    };

	void setStatus_vector(std::vector<std::string> vect)
	{
		this->status_vector = vect;
	}

    void setStatus(Request *request, parse_config *config)
    {
		(void)request;
		(void)config;
        // this->status = status;
		if (!request->get_is_formated())
		{
			if (request->get_transfer_encoding() != "chunked")
				this->status = "501 NOT IMPLEMENTED";
			else if (request->get_transfer_encoding() == "chunked" &&
				request->getcontent_length() == -1)
				this->status = "400 BAD REQUEST";
			else if (url_parser(request->geturl()))
				this->status = "400 BAD REQUEST";
			else if (request->getcontent_length() > 2048)
				this->status = "414 REQUEST-URI TOO LARGE";
			else if (request->body_length() > request->getcontent_length())
				this->status = "413 REQUEST ENTITY TOO LARGE";
		}
		// else
		// {
		// 	if (!url_macth())
		// 		this->status = "400 BAD REQUEST";
		// 	else if (url_have_redir())
		// 		this->status = "301 MOVED PERMANENTLY";
		// 	else if ((std::string request->get_method()))
		// }

        std::cout << "- Set Status : " << this->status << std::endl;
	}

	bool url_parser(std::string url)
	{
		std::string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
		for(std::string::iterator it = url.begin(); it != url.end(); ++it)
			if (allowed.find(*it) == std::string::npos)
				return false;
		return (true);
	}

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
        // res.https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set("Content-Type: ");
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