#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"

void Response::setStatus(Request *request, Server server)
{
	if (!request->get_is_formated())
	{
        
		if (request->get_transfer_encoding().size() > 0 && request->get_transfer_encoding() != "chunked") // done 
			this->status = "501 NOT IMPLEMENTED\r\n";

		else if (!request->get_transfer_encoding().size() && request->getcontent_length() <= 0 &&  request->getMethod() == "POST") // done // 405 not allowed in nginx 
            this->status = "400 BAD REQUEST\r\n";

		else if (!url_is_formated(request->geturl())) // done
            this->status = "400 BAD REQUEST\r\n";

        else if (request->geturl().length() > MAX_URL_LENGTH)  // done // nginx accepte more than 2048 char 
			this->status = "414 REQUEST-URI TOO LARGE\r\n";

        else if ( request->get_body_length() > server.get_client_max_body_size()) // almost done
        	this->status = "413 REQUEST ENTITY TOO LARGE\r\n";

    }
    std::cout << "NOT WORKING AS EXCPECTED FUNCTION DOWN HERE"  << std::endl;
    if (!get_matched_location_for_request_uri(request->geturl(), server)) // NOT WORKING TRASH
        this->status = "-404 NOT FOUND\r\n";
    // else if (url_redirected(request->geturl(), server))
    //     this->status = "301 MOVED PERMANENTLY\r\n";
    
    // else if (!method_is_allowed(request->getMethod(), request->geturl(),  server))
    //     this->status = "405 METHOD NOT ALLOWED\r\n";
    // }
    
    std::cout << red << "-> Set Status : " << this->status << def  << std::endl;
   
}

// void Response::setContentType(char *path)
// {
//     std::string s(path);
//     std::string s1 = s.substr(s.find_last_of(".") + 1);
//     if (s1 == "html")
//         this->contentType = "text/html";
//     else if (s1 == "css")
//         this->contentType = "text/css";
//     else if (s1 == "js")
//         this->contentType = "application/javascript";
//     else if (s1 == "jpg")
//         this->contentType = "image/jpeg";
//     else if (s1 == "png")
//         this->contentType = "image/png";
//     else if (s1 == "gif")
//         this->contentType = "image/gif";
//     else if (s1 == "ico")
//         this->contentType = "image/x-icon";
//     else if (s1 == "svg")
//         this->contentType = "image/svg+xml";
//     else if (s1 == "mp3")
//         this->contentType = "audio/mpeg";
//     else if (s1 == "mp4")
//         this->contentType = "video/mp4";
//     else if (s1 == "ogg")
//         this->contentType = "audio/ogg";
//     else if (s1 == "ogv")
//         this->contentType = "video/ogg";
//     else if (s1 == "wav")
//         this->contentType = "audio/wav";
//     else if (s1 == "webm")
//         this->contentType = "video/webm";
//     else if (s1 == "txt")
//         this->contentType = "text/plain";
//     std::cout << red << "- Set Content-Type : " << s << green << " " << s1 << def << std::endl;
// };

std::vector<char> Response::get_buffer()
{
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

std::string Response::getHeader() 
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
    if (tt > 0)
    {
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
    setHeader(res);

    return res;
};

//
// void ERRORresponse(Request *request, Response *response)
// {
//     (void)request;
//     (void)response;
//     std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
// }

Response  response( Request *request, ParseConfig *config, int index_server)
{
    std::cout << blue << "********** { Procces Response } ***********************" << def << std::endl;
    Response response;

    /* is_req_well_formed() */

    response.setStatus(request, config->get_server_vect()[index_server]);
    // while (1)
    // {
    //     ;
    // }
    // if () there is an error status go fill the response body with the error html page
    //     ERRORresponse(request, &response);
    // if () the response is good ....
    /**/
    // if (!(request->getMethod().compare("GET")))
    //     GETresponse(request, &response, config, index_server);
    // else if (request->getMethod().compare("POST") == 0)
    //     POSTresponse(request, &response, config, index_server);
    // else if (request->getMethod().compare("DELETE") == 0)
    //     DELETEresponse(request, &response, config, index_server);
    // std::cout << blue << "********** {End Procces Response } ******************" << def << std::endl ;

    return response;
}

// void DELETEresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
// {
//     (void)request;
//     (void)response;
//     (void)config;
//     (void)index_server;
//     // std::cout << "im doing delete response\n";
//     // response->set_location_url(get_location_url(request->geturl(),
//     //     config->get_server_vect()[index_server]));

//     // if(!requested_file_in_root(request->geturl(),
//     //     config->get_server_vect()[index_server]))
//     // {
//     //     response->setStatus("404 NOT FOUND");
//     // }
//     // else if (!is_file(request->geturl()))
//     // {
//     //     if (request->geturl().back() != '/')
//     //     {
//     //         response->setStatus("409 CONFLICT");
//     //     }
//     //     else
//     //     {
//     //         if (Location_have_cgi(request->geturl()))
//     //         {
//     //             if (file_exist(request->geturl() + "index.html"))
//     //                 response->setStatus("403 FORBIDDEN");
//     //             else
//     //                  cgi_response(request, response, config, index_server);
//     //         }
//     //         else
//     //         {
//     //             if (delete_directory(request->geturl()))
//     //                 response->setStatus("204 NO CONTENT");
//     //             else if (has_write_permission(request->geturl()))
//     //                 response->setStatus("403 FORBIDDEN");
//     //             else
//     //                 response->setStatus("500 INTERNAL SERVER ERROR");
//     //         }
//     //     }
//     // }
//     // else
//     // {
//     //     if (Location_have_cgi(request->geturl()))
//     //          cgi_response(request, response, config, index_server);
//     //     else
//     //     {
//     //         if (delete_file(request->geturl()))
//     //             response->setStatus("204 NO CONTENT");
//     //     }
//     // }
// }

// void POSTresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
// {
//     (void)request;
//     (void)response;
//     (void)config;
//     (void)index_server;
//     // response->set_location(get_location_url(request->geturl(),
//     //     config->get_server_vect()[index_server]));
//     // if(Location_support_upload(request->geturl(),
//     //         config->get_server_vect()[index_server]))
//     // {
//     //     // upload_response(request, response, config, index_server);
//     // }
//     // else
//     // {
//     //     if (!requested_file_in_root(request->geturl(),
//     //         config->get_server_vect()[index_server]))
//     //     {
//     //         response->setStatus("404 NOT FOUND");
//     //     }
//     //     else
//     //     {
//     //         if (is_file(request->geturl()))
//     //         {
//     // //             if (Location_have_cgi(request->geturl()))
//     // //                 cgi_response(request, response, config, index_server);
//     // //             else
//     // //                 response->setStatus("405 METHOD NOT ALLOWED");
//     //         }
//     //         else
//     //         {
//     //             if (request->geturl().back() != '/')
//     //                 response->setStatus("301 MOVED PERMANENTLY");
//     //             else if (!file_exist(request->geturl() + "index.html"))
//     //                 response->setStatus("403 FORBIDDEN");
//     // //             else
//     // //                 if (Location_have_cgi(request->geturl()))
//     // //                     cgi_response(request, response, config, index_server);
//     // //                 else
//     // //                     response->setStatus("403 FORBIDDEN");
//     //         }
//     //     }
//     // }
//     std::cout << "im doing post response\n";
// }

// void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
// {
//     (void)request;
//     (void)response;
//     (void)config;
//     (void)index_server;
//     /////// main process to set a good response : set mandatory headers + set path of file to send
//     // response->set_location(get_location_url(request->geturl(),
//     //     config->get_server_vect()[index_server]));

//     // response->set_redirection(get_redirection_url(request->geturl(),
//     //     config->get_server_vect()[index_server]));

//     // if(!requested_file_in_root(request->geturl(),
//     //     config->get_server_vect()[index_server]))
//     // {
//     //     response->setStatus("404 NOT FOUND");
//     // }
//     // else if (request->geturl().back() != '/')
//     // {
//     //     response->setStatus("301 MOVED PERMANENTLY");
//     // }
//     // else
//     // {
//     //     if (!file_exist(request->geturl() + "index.html"))
//     //     {
//     //         if (config->get_server_vect()[index_server].get_autoindex())
//     //         {
//     //             // get_autoindex();
//     //             response->setStatus("200 OK");
//     //         }
//     //         else
//     //         {
//     //           response->setStatus("403 FORBIDDEN");
//     //         }
//     //     }
//     //     else
//     //     {
//     //         if (is_file(request->geturl()))
//     //         {
//     //             // if (Location_have_cgi(request->geturl()))
//     //             // {
//     //             //     // get_cgi();
//     //             // }
//     //             // else
//     //             // {
//     //             //     response->setStatus("200 OK");
//     //             //     // request_file
//     //             // }
//     //         }
//     //     }
//     // }
    
//     // std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
//     // if (true)
//     // {
//     //     std::string path ;// = (char *)malloc(sizeof(char) * (1000));
//     //     path.reserve(10000);
//     //     path = config->get_server_vect()[index_server].get_root();
//     //     if (request->getPath() == "/")
//     //         path.append("index.html");
//     //     else
//     //         path.append(request->getPath());

//     //     std::cout << B_blue << "GET from File: " << path << B_def << std::endl;

//     //     FILE *pFile;
//     //     pFile = fopen(path.c_str(), "r");
//     //     char s2[50];
//     //     if (pFile != NULL)
//     //     {
//     //         /////// main process to set a good response : set mandatory headers + set path of file to send
//     //         strcpy(s2, " 200 OK\r\n");
//     //         response->setResponseStatus(s2);
//     //         response->setResponseHeader();
//     //         response->setContentType((char*)path.c_str());
//     //         response->setpath(path);
//     //     }
//     //     else
//     //     {
//     //         /////// main process to set a good response : set mandatory headers + set path of file to send
//     //         char ss[100] = "./errorsPages/404/404.html";
//     //         strcpy(s2, " 404 NOT FOUND\r\n");
//     //         response->setResponseStatus(s2);
//     //         response->setResponseHeader();
//     //         response->setContentType(ss);
//     //         response->setpath(ss);

//     //     }
//     //     fclose(pFile);
//     // }
// }

// void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
// {
//     (void )index_server;
//     /* This function is about to form other headers and set the body to read later */
//     std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
//     if (!request->getPath().empty())
//     {
//         char *path = (char *)malloc(sizeof(char) * (1000));
//         std::cout << B_red << "root path = {" << config->get_server_vect()[index_server].get_root().c_str() << "}" << B_def << std::endl;
//         strcpy(path, config->get_server_vect()[index_server].get_root().c_str());
//         struct stat st;
//         if (request->getPath() == "/")
//             strcpy(path + (strlen(path)), "index.html");
//         else
//             strcpy(path + (strlen(path) - 1), request->getPath().c_str());
//         stat(path, &st);
//         char s2[50];
//         /* this status forme to be removed */
//         if (st.st_size > 0 && open(path, O_RDONLY) > 0)
//         {
//             strcpy(s2, "200 OK\r\n");
//             response->setResponseStatus(s2);
//             response->setResponseHeader();
//             response->setContentType(path);
//             response->setbody_file_size(getFileSize(path));
//             // response->setBody(read_by_vector(path, response));
//             response->setpath(path);
//             std::cout << B_blue << "getFileSize(path): " << getFileSize(path) << B_def << std::endl;
//         }
//         else
//         {
//             response->set_requestFuckedUp(true);
//             strcpy(s2, "404 Not Found\r\n");
//             char _path[100] = "./errorsPages/404/404.html";
//             response->setResponseStatus(s2);
//             response->setResponseHeader();
//             response->setContentType(_path);
//             response->setbody_file_size(getFileSize(_path));
//             // response->setBody(read_by_vector(path, response));
//             response->setpath(_path);
//             std::cout << B_blue << "getFileSize(path): " << getFileSize(_path) << B_def << std::endl;
//         }
//         free(path);
//     }
// }

void response(int new_socket, Request *request, ParseConfig *config, int index_server)
{
    // Response response;
	// response.setStatus(request, config->get_server_vect()[index_server]);
    std::cout << blue << "********** { Response } ***********************" << def << std::endl;
    // // if (!request->isGoodrequest())
    // //     ERRORresponse(request, &response);
    // if (!(request->getMethod().compare("GET")))
    //     GETresponse(request, &response, config, index_server);
    // else if (request->getMethod().compare("POST") == 0)
    //     POSTresponse(request, &response, config, index_server);
    // else if (request->getMethod().compare("DELETE") == 0)
    //     DELETEresponse(request, &response, config, index_server);
    // else
    //     ERRORresponse(request, &response);
    // // std::cout << blue << "********** {End Response } ******************" << def << std::endl
    // //           << std::endl;
    // std::string str = response.getResponse();
    // size_t lenght = str.size();
    // ssize_t size_send = send(new_socket, str.c_str(), lenght, MSG_OOB);
    // if (size_send > 0)
    //     std::cout << B_green << "********** data size send {" << size_send << "}******************" << B_def << std::endl;
    // else
    //     std::cout << B_red << "********** no data t send {" << size_send << "}******************" << B_def << std::endl;
}