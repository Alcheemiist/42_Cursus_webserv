#include "../elements.hpp"


void GETresponse(Request *request, Response *response, Config *config)
{
    (void )config;
    Color::Modifier red(Color::FG_RED);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier B_red(Color::BG_RED);
    
    std::cout << blue << "********** Response Data ***********************" << def  << std::endl;
    std::cout << "*- requeste file->" << request->getPath() << std::endl;
    std::cout << "*- config D_file->" << config->getDefaultpath() << std::endl;

    if (request->getPath() == "./www/index.html")
    {
        response->generateResponse(request);
    }
    std::cout << blue << "********** End  Response Data ******************" << def << std::endl;
}

void POSTresponse()
{
    std::cout << "im doing post response\n";
}

void PUTresponse()
{
    std::cout << "im doing put response\n";
}

void DELETEresponse()
{
    std::cout << "im doing delete response\n";
}

void HEADresponse()
{
    std::cout << "im doing head response\n";
}

void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << "im doing error response\n";
}

void response(int new_socket, Request request, Config *config)
{
    Response response;

    if (request.isGoodrequest())
        ERRORresponse(&request, &response);
    if (!(request.getMethod().compare("GET")))
        GETresponse(&request, &response, config);
    else if (request.getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request.getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        HEADresponse();
    send(new_socket, response.getResponse(), response.getSize(), MSG_OOB);
}
