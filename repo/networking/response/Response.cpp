#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>

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
