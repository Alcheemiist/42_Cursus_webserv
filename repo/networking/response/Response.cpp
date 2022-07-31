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

bool Response::url_is_formated(std::string url)
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

void Response::setStatus(std::string status)
{
    this->status = status;
}

void Response::setStatus(Request *request, Server server)
{
	if (!request->get_is_formated())
	{
		if (request->get_transfer_encoding() != "" &&
                request->get_transfer_encoding() != "chunked")
			this->status = "501 NOT IMPLEMENTED";
		else if (request->get_transfer_encoding() == "" &&
			    request->getcontent_length() == -1 &&
                request->getMethod() == "POST")
			this->status = "400 BAD REQUEST";
		else if (url_is_formated(request->geturl()))
			this->status = "400 BAD REQUEST";
		else if (request->geturl().length() > MAX_URL_LENGTH)
			this->status = "414 REQUEST-URI TOO LARGE";
		// else if (request->body_length() > request->getcontent_length())
		// else if (check_max_body_length(request->getbody_length(),
        //             request->getcontent_length()))
		// 	this->status = "413 REQUEST ENTITY TOO LARGE";
	}
	else
	{
		if (get_matched_location_for_request_uri(request->geturl(), server))
			this->status = "404 NOT FOUND";
		else if (url_redirected(request->geturl(), server))
			this->status = "301 MOVED PERMANENTLY";
		else if (!method_is_allowed(request->getMethod(), request->geturl(),
                    server))
            this->status = "405 METHOD NOT ALLOWED";
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

void Response::set_redirection_url(std::string url)
{
    this->redirection_url = url;
}

void Response::set_location_url(std::string url)
{
    this->location_url = url;
}

std::string Response::get_redirection_url()
{
    return this->redirection_url;
}

std::string Response::get_location_url()
{
    return this->location_url;
}


void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
}

void DELETEresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    (void)request;
    (void)response;
    (void)config;
    (void)index_server;
    // std::cout << "im doing delete response\n";
    // response->set_location_url(get_location_url(request->geturl(),
    //     config->get_server_vect()[index_server]));

    // if(!requested_file_in_root(request->geturl(),
    //     config->get_server_vect()[index_server]))
    // {
    //     response->setStatus("404 NOT FOUND");
    // }
    // else if (!is_file(request->geturl()))
    // {
    //     if (request->geturl().back() != '/')
    //     {
    //         response->setStatus("409 CONFLICT");
    //     }
    //     else
    //     {
    //         if (Location_have_cgi(request->geturl()))
    //         {
    //             if (file_exist(request->geturl() + "index.html"))
    //                 response->setStatus("403 FORBIDDEN");
    //             else
    //                  cgi_response(request, response, config, index_server);
    //         }
    //         else
    //         {
    //             if (delete_directory(request->geturl()))
    //                 response->setStatus("204 NO CONTENT");
    //             else if (has_write_permission(request->geturl()))
    //                 response->setStatus("403 FORBIDDEN");
    //             else
    //                 response->setStatus("500 INTERNAL SERVER ERROR");
    //         }
    //     }
    // }
    // else
    // {
    //     if (Location_have_cgi(request->geturl()))
    //          cgi_response(request, response, config, index_server);
    //     else
    //     {
    //         if (delete_file(request->geturl()))
    //             response->setStatus("204 NO CONTENT");
    //     }
    // }
}

void POSTresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    (void)request;
    (void)response;
    (void)config;
    (void)index_server;
    // response->set_location_url(get_location_url(request->geturl(),
    //     config->get_server_vect()[index_server]));
    // if(Location_support_upload(request->geturl(), config->get_server_vect()[index_server]))
    // {
    //     upload_response(request, response, config, index_server);
    // }
    // else
    // {
    //     if (!requested_file_in_root(request->geturl(),
    //         config->get_server_vect()[index_server]))
    //     {
    //         esponse->setStatus("404 NOT FOUND");
    //     }
    //     else
    //     {
    //         if (is_file(request->geturl()))
    //         {
    //             if (Location_have_cgi(request->geturl()))
    //                 cgi_response(request, response, config, index_server);
    //             else
    //                 response->setStatus("405 METHOD NOT ALLOWED");
    //         }
    //         else
    //         {
    //             if (request->geturl().back() != '/')
    //                 response->setStatus("301 MOVED PERMANENTLY");
    //             else if (!file_exist(request->geturl() + "index.html"))
    //                 response->setStatus("403 FORBIDDEN");
    //             else
    //                 if (Location_have_cgi(request->geturl()))
    //                     cgi_response(request, response, config, index_server);
    //                 else
    //                     response->setStatus("403 FORBIDDEN");
    //         }
    //     }
    // }
    std::cout << "im doing post response\n";
}

void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    response->set_location_url(get_location_url(request->geturl(),
        config->get_server_vect()[index_server]));

    response->set_redirection_url(get_redirection_url(request->geturl(),
        config->get_server_vect()[index_server]));

    if(!requested_file_in_root(request->geturl(),
        config->get_server_vect()[index_server]))
    {
        response->setStatus("404 NOT FOUND");
    }
    else if (request->geturl().back() != '/')
    {
        response->setStatus("301 MOVED PERMANENTLY");
    }
    else
    {
        if (!file_exist(request->geturl() + "index.html"))
        {
            if (config->get_server_vect()[index_server].get_autoindex())
            {
                // get_autoindex();
                response->setStatus("200 OK");
            }
            else
            {
              response->setStatus("403 FORBIDDEN");
            }
        }
        else
        {
            if (is_file(request->geturl()))
            {
                // if (Location_have_cgi(request->geturl()))
                // {
                //     // get_cgi();
                // }
                // else
                // {
                //     response->setStatus("200 OK");
                //     // request_file
                // }
            }
        }
    }

    std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
    if (!request->getPath().empty())
    {
        std::string path;// = (char *)malloc(sizeof(char) * (1000));
        path = config->get_server_vect()[index_server].get_root();
        if (request->getPath() == "/")
            path += "index.html";
        else
            path += request->getPath();
        std::cout << B_blue << "GET from File: " << path << B_def << std::endl;

        FILE *pFile;
        pFile = fopen(path.c_str(), "r");
        char s2[50];
        if (pFile != NULL)
        {
            strcpy(s2, " 200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType((char*)path.c_str());
            response->setBody(readFile(path.c_str()));
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

void response(int new_socket, Request *request, ParseConfig *config, int index_server)
{
    Response response;

	response.setStatus(request, config->get_server_vect()[index_server]);
    // std::cout << blue << "********** { Response } ***********************" << def << std::endl;
    // if (!request->isGoodrequest())
    //     ERRORresponse(request, &response);
    if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse(request, &response, config, index_server);
    else
        ERRORresponse(request, &response);
    // std::cout << blue << "********** {End Response } ******************" << def << std::endl
    //           << std::endl;
    std::string str = response.getResponse();
    size_t lenght = str.size();
    ssize_t size_send = send(new_socket, str.c_str(), lenght, MSG_OOB);
    if (size_send > 0)
        std::cout << B_green << "********** data size send {" << size_send << "}******************" << B_def << std::endl;
    else
        std::cout << B_red << "********** no data t send {" << size_send << "}******************" << B_def << std::endl;
}
