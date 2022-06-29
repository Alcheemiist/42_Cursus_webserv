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
    std::cout << B_red << "im doing error response status= " << red <<  request->getRequestStatus() << def << std::endl;
}

void DELETEresponse()
{
    std::cout << "im doing delete response\n";
}

void POSTresponse()
{
    std::cout << "im doing post response\n";
}

void GETresponse(Request *request, Response *response, Config *config)
{
    (void )config;
    Color::Modifier red(Color::FG_RED);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier B_red(Color::BG_RED);
    Color::Modifier B_green(Color::BG_GREEN);
    Color::Modifier B_blue(Color::BG_BLUE);

    std::cout << B_green << "IM DOING GET REQUEST" << def << std::endl;


    std::cout << blue << "********** Response Data ***********************" << def  << std::endl;
    
    
    
    std::cout <<  B_green << "*- requeste file-> " << request->getPath() << def << std::endl;
    std::cout <<  B_green << "*- config D_file-> " << config->getDefaultpath()  << def << std::endl;

    if (request->getPath() == "/")
    {
        char *path = (char *)malloc(sizeof(char) * (strlen(config->getDefaultpath().c_str()) + 1) + strlen("/index.html"));
        
        strcpy(path, ".");
        strcpy(path, config->getDefaultpath().c_str());
        strcpy(path, "/index.html");
        
        if (open(path, O_RDONLY) < 0)
        {
            response->setResponseStatus(" 404 OK\r\n");
            response->setResponseHeader();
            response->setBody("<h1>404 Not Found</h1>");
        }
        else
        {
            response->setResponseStatus(" 200 OK\r\n");
            response->setResponseHeader();
            response->setBody(readFile(path));
        }
    }





    std::cout << blue << "********** End  Response Data ******************" << def << std::endl;





}

void response(int new_socket, Request request, Config *config)
{
    Response response;

    if (!request.isGoodrequest())
        ERRORresponse(&request, &response);
    else if (!(request.getMethod().compare("GET")))
        GETresponse(&request, &response, config);
    else if (request.getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request.getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        ERRORresponse(&request, &response);

    send(new_socket, response.getResponse(), response.size(), MSG_OOB);
}
