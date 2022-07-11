#include "../elements.hpp"
#include <fcntl.h>

void ERRORresponse(Request *request, Response *response)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier B_red(Color::BG_RED);

    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << red << request->getRequestStatus() << def << std::endl;
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
    Color::Modifier red(Color::FG_RED);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier B_red(Color::BG_RED);
    Color::Modifier B_green(Color::BG_GREEN);
    Color::Modifier B_blue(Color::BG_BLUE);
    Color::Modifier B_def(Color::BG_DEFAULT);

    std::cout << B_red << "IM DOING GET REQUEST" << B_def << std::endl;

    std::cout << blue << "********** Response Data ***********************" << def << std::endl;
    std::cout << B_green << "*- requeste file-> " << request->getPath() << B_def << std::endl;

    if (request->getPath() == "/")
    {
        char *path = (char *)malloc(sizeof(char) * (strlen(config[0].get_server_vect()[0].get_root().c_str()) + 1) + strlen("/index.html"));
        strcpy(path, config[0].get_server_vect()[0].get_root().c_str());
        strcpy(path + (strlen(path)), "index.html");
        std::cout << B_blue << " body file path : " << path << B_def << std::endl;

        if (open(path, O_RDONLY) < 0)
        {
            char s[20];
            strcpy(s, " 404 OK\r\n");
            response->setResponseStatus(s);
            response->setResponseHeader();
            char s1[20];
            strcpy(s1, "<h1>404 Not Found</h1>");
            response->setBody(s1);
        }
        else
        {
            std::cout << green << " prepare 200 response" << def << std::endl;
            char s2[20];
            strcpy(s2, " 200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setBody(readFile(path));
            // std::cout << "reading file : " << readFile(path) << std::endl;
        }
    }

    std::cout << blue << "********** End  Response Data ******************" << def << std::endl;
}

void response(int new_socket, Request *request, parse_config *config)
{
    Response response;

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

    // std::cout << response.getResponse().c_str() << std::endl;
    ssize_t size_send = send(new_socket, response.getResponse().c_str(), response.getResponse().length(), MSG_OOB);
    std::cout << "size send: " << size_send << std::endl;
}
