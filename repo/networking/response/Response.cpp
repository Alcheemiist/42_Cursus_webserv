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

void GETresponse(Request *request, Response *response, parse_config *config)
{
    std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
    if (!request->getPath().empty())
    {
        char *path = (char *)malloc(sizeof(char) * (1000));
        strcpy(path, config[0].get_server_vect()[0].get_root().c_str());

        // default path file in root directory
        if (request->getPath() == "/")
            strcpy(path + (strlen(path)), "index.html");
        else
            strcpy(path + (strlen(path) - 1), request->getPath().c_str());

        std::cout << B_blue << "GET from File: " << path << B_def << std::endl;
        FILE *pFile;
        pFile = fopen(path, "r");
        if (pFile != NULL)
        {
            /* making of response from path file existe */
            char s2[50];
            strcpy(s2, " 200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();

            response->setBody(readFile(path));
        }
        else
        {
            /* path not found */
            char s[50];
            strcpy(s, " 404 OK\r\n");
            response->setResponseStatus(s);
            response->setResponseHeader();
            char s1[50];
            strcpy(s1, "<h1>404 Not Found</h1>");
            response->setBody(s1);
        }
        fclose(pFile);
    }
}

void response(int new_socket, Request *request, parse_config *config)
{
    Response response;

    std::cout << blue << "********** Response Data ***********************" << def << std::endl;

    if (!request->isGoodrequest())
        ERRORresponse(request, &response);
    else if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        ERRORresponse(request, &response);

    std::cout << blue << "********** End Response Data ******************" << def << std::endl;

    ssize_t size_send = send(new_socket, response.getResponse().c_str(), response.getResponse().length(), MSG_OOB);

    if (size_send >= 0)
        std::cout << B_green << "********** data size send : " << size_send << "******************" << B_def << std::endl;
    else
        std::cout << B_red << "********** data size send : " << size_send << "******************" << B_def << std::endl;
}
