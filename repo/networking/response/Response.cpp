#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"

void Response::setStatus(Request *request, Server server)
{
	if (!request->get_is_formated())
	{
		if (request->get_transfer_encoding() != "" &&
                request->get_transfer_encoding() != "chunked")
			this->status = "501 NOT IMPLEMENTED\r\n";
		else if (request->get_transfer_encoding() == "" &&
			    request->getcontent_length() == -1 &&
                request->getMethod() == "POST")
			this->status = "400 BAD REQUEST\r\n";
		else if (url_is_formated(request->geturl()))
			this->status = "400 BAD REQUEST\r\n";
		else if (request->geturl().length() > MAX_URL_LENGTH)
			this->status = "414 REQUEST-URI TOO LARGE\r\n";
		// else if (request->body_length() > request->getcontent_length())
		// else if (check_max_body_length(request->getbody_length(),
        //             request->getcontent_length()))
		// 	this->status = "413 REQUEST ENTITY TOO LARGE\r\n";
	}
	else
	{
		if (get_matched_location_for_request_uri(request->geturl(), server))
			this->status = "404 NOT FOUND\r\n";
		else if (url_redirected(request->geturl(), server))
			this->status = "301 MOVED PERMANENTLY\r\n";
		else if (!method_is_allowed(request->getMethod(), request->geturl(),
                    server))
            this->status = "405 METHOD NOT ALLOWED\r\n";
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

char *readAllFile(char *path)
{
    char buffer[DATA_BUFFER_SIZE];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);
    int max_size = getFileSize(path);
    int fd = open(path, O_RDWR);
    size_t read_size = 0;

    if (fd < 0)
        throw std::runtime_error("Error opening file");
    else
        for (int i = 0; i < max_size; i += DATA_BUFFER_SIZE)
        {
            int size = read(fd, buffer, DATA_BUFFER_SIZE);
            for (int i = 0; i < size; i++)
                return_buffer[i + read_size] = buffer[i];
            if (size == 0)
                break;
            if (size < 0)
                throw std::runtime_error("Error reading file");
            read_size += size;
        }
        return_buffer[max_size] = '\0';
    return return_buffer;
}

size_t _getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
}


std::vector<char> read_by_vector(char *path, Response *response)
{
    int fd = open(path, O_RDWR), n = -1;
    char *_buffer = (char *)malloc(sizeof(char) * (response->getbody_file_size() + 1));
    std::vector<char> buffer;

    if (fd < 0)
        perror("Error opening file");
    else
    {
        for (int i = 0; i < response->getbody_file_size(); i += n)
        {
            if ((n = read(fd, _buffer + i, (1024 * 16))) < 0)
                break;
        }
        buffer.insert(buffer.begin(), _buffer, _buffer + response->getbody_file_size());
    }
    close(fd);
    return buffer;
}

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

Response response(int new_socket, Request *request, ParseConfig *config, int index_server)
{
    (void)new_socket;
    Response response;

    std::cout << blue << "********** { Procces Response } ***********************" << def << std::endl;
	
    /* is_req_well_formed() */
    response.setStatus(request, config->get_server_vect()[index_server]);
    
    // if () there is an error status go fill the response body with the error html page  
    //     ERRORresponse(request, &response);
    // if () the response is good ....
    /**/
    
    if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse(request, &response, config, index_server);
    std::cout << blue << "********** {End Procces Response } ******************" << def << std::endl ;
    return response;
}


void DELETEresponse(Request *request, Response *response, ParseConfig *config,
                        int index_server)
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

void POSTresponse(Request *request, Response *response, ParseConfig *config,
                    int index_server)
{
    response->set_location(get_location_url(request->geturl(),
        config->get_server_vect()[index_server]));
    if(Location_support_upload(request->geturl(),
            config->get_server_vect()[index_server]))
    {
        // upload_response(request, response, config, index_server);
    }
    else
    {
        if (!requested_file_in_root(request->geturl(),
            config->get_server_vect()[index_server]))
        {
            response->setStatus("404 NOT FOUND");
        }
        else
        {
            if (is_file(request->geturl()))
            {
    //             if (Location_have_cgi(request->geturl()))
    //                 cgi_response(request, response, config, index_server);
    //             else
    //                 response->setStatus("405 METHOD NOT ALLOWED");
            }
            else
            {
                if (request->geturl().back() != '/')
                    response->setStatus("301 MOVED PERMANENTLY");
                else if (!file_exist(request->geturl() + "index.html"))
                    response->setStatus("403 FORBIDDEN");
    //             else
    //                 if (Location_have_cgi(request->geturl()))
    //                     cgi_response(request, response, config, index_server);
    //                 else
    //                     response->setStatus("403 FORBIDDEN");
            }
        }
    }
    std::cout << "im doing post response\n";
}

void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    response->set_location(get_location_url(request->geturl(),
        config->get_server_vect()[index_server]));

    response->set_redirection(get_redirection_url(request->geturl(),
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

// void response(int new_socket, Request *request, ParseConfig *config, int index_server)
// {
//     Response response;
// 	response.setStatus(request, config->get_server_vect()[index_server]);
//     // std::cout << blue << "********** { Response } ***********************" << def << std::endl;
//     // if (!request->isGoodrequest())
//     //     ERRORresponse(request, &response);
//     if (!(request->getMethod().compare("GET")))
//         GETresponse(request, &response, config, index_server);
//     else if (request->getMethod().compare("POST") == 0)
//         POSTresponse(request, &response, config, index_server);
//     else if (request->getMethod().compare("DELETE") == 0)
//         DELETEresponse(request, &response, config, index_server);
//     else
//         ERRORresponse(request, &response);
//     // std::cout << blue << "********** {End Response } ******************" << def << std::endl
//     //           << std::endl;
//     std::string str = response.getResponse();
//     size_t lenght = str.size();
//     ssize_t size_send = send(new_socket, str.c_str(), lenght, MSG_OOB);
//     if (size_send > 0)
//         std::cout << B_green << "********** data size send {" << size_send << "}******************" << B_def << std::endl;
//     else
//         std::cout << B_red << "********** no data t send {" << size_send << "}******************" << B_def << std::endl;
// }