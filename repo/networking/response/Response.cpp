#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"

Response::Response() : version("HTTP/1.1 "), status("200 OK\r\n"), header(""),
    body(""), response(""), responseStatus(""), body_length(0), contentType(""){};

void Response::setVersion(std::string version)
{
    this->version = version;
    std::cout << "- Set Version : " << this->version << std::endl;
};

bool Response::url_parser(std::string url)
{
	std::string allowed ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
	for(std::string::iterator it = url.begin(); it != url.end(); ++it)
		if (allowed.find(*it) == std::string::npos)
			return false;
	return (true);
}

void Response::setHeader(char *header)
{
    this->header = header;
    std::cout << "- Set header : " << this->header << std::endl;
};

void Response::setBody(char *body)
{
    this->body = body;
    std::cout << "- Set body of size = " << this->body.size() << std::endl;
}

void Response::setResponseStatus(char *status)
{
    this->responseStatus = status;
    std::cout << "- Set Status : " << this->responseStatus << std::endl;
};

size_t Response::size() const { return body_length; };

void Response::setResponseHeader()
{
    this->header = this->version.c_str();
    std::cout << "- Set Version : " << this->version << std::endl;
};

std::string Response::getResponse() const
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

void Response::setStatus(Request *request, parse_config *config)
{
	(void)request;
	(void)config;
    // this->status = status;
	if (!request->get_is_formated())
	{
		if (request->get_transfer_encoding() != "" &&
                request->get_transfer_encoding() != "chunked")
			this->status = "501 NOT IMPLEMENTED";
		else if (request->get_transfer_encoding() == "" &&
			    request->getcontent_length() == -1 &&
                request->getMethod() == "POST")
			this->status = "400 BAD REQUEST";
		else if (url_parser(request->geturl()))
			this->status = "400 BAD REQUEST";
		else if (request->geturl().length() > 2048)
			this->status = "414 REQUEST-URI TOO LARGE";
		else if (request->body_length() > request->getcontent_length())
			this->status = "413 REQUEST ENTITY TOO LARGE";
	}
	else
	{
		if ((this->location = url_matched(request->geturl(), request->get_port(),
                config->get_server_vect())) == "")
			this->status = "400 BAD REQUEST";
		// else if (url_have_redir())
		// 	this->status = "301 MOVED PERMANENTLY";
		// else if ((std::string request->get_method()))
	}
    std::cout << "- Set Status : " << this->status << std::endl;
}

void Response::setContentType(char *path)
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

void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
}

void DELETEresponse()
{
    std::cout << "im doing delete response\n";
}

void POSTresponse()
{
    std::cout << "im doing post response\n";
}

void GETresponse(Request *request, Response *response, parse_config *config, int index_server)
{
    std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
    if (!request->getPath().empty())
    {
        char *path = (char *)malloc(sizeof(char) * (1000));
        std::cout << B_red << "root path = {" << config->get_server_vect()[index_server].get_root().c_str() << "}" << B_def << std::endl;
        strcpy(path, config->get_server_vect()[index_server].get_root().c_str());
        if (request->getPath() == "/")
            strcpy(path + (strlen(path)), "index.html");
        else
            strcpy(path + (strlen(path) - 1), request->getPath().c_str());
        std::cout << B_blue << "GET from File: " << path << B_def << std::endl;

        FILE *pFile;
        pFile = fopen(path, "r");
        char s2[50];
        if (pFile != NULL)
        {
            strcpy(s2, " 200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(path);
            response->setBody(readFile(path));
        }
        else
        {
            char ss[100] = "./errorsPages/404/404.html";
            strcpy(s2, " 404 NOT FOUND\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(ss);
            response->setBody(readFile("./errorsPages/404/404.html"));
        }
        fclose(pFile);
    }
}

void response(int new_socket, Request *request, parse_config *config, int index_server)
{
    Response response;

	response.setStatus(request, config);
    std::cout << blue << "********** { Response } ***********************" << def << std::endl;
    if (!request->isGoodrequest())
        ERRORresponse(request, &response);
    else if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        ERRORresponse(request, &response);
    std::cout << blue << "********** {End Response } ******************" << def << std::endl
              << std::endl;
    std::string str = response.getResponse();
    size_t lenght = str.size();
    ssize_t size_send = send(new_socket, str.c_str(), lenght, MSG_OOB);
    if (size_send > 0)
        std::cout << B_green << "********** data size send {" << size_send << "}******************" << B_def << std::endl;
    else
        std::cout << B_red << "********** no data t send {" << size_send << "}******************" << B_def << std::endl;
}
