#include "./elements.hpp"

Color::Modifier red(Color::FG_RED);
Color::Modifier def(Color::FG_DEFAULT);
Color::Modifier blue(Color::FG_BLUE);
Color::Modifier green(Color::FG_GREEN);
Color::Modifier B_red(Color::BG_RED);

// REQUEST --------------------------------------------------
long readRequest(int new_socket, Request *request)
{
    char buffer[30000];
    request->setbytes( recv(new_socket, buffer, 3000000 , 0) );
    std::cout << red << "\n----(Request*" << request->getRequestNum() << ")------- (N Bytes: " << request->getbytes() << ")------------ "<< def << std::endl;
    std::cout << buffer << std::endl;
    std::cout << red << "-------------------------------------------------" << def << std::endl;
    return request->getbytes();
}

void checkRequest(Request *request)
{
    (void)request;
}

// RESPONSE --------------------------------------------------
char *readFile(const char * fileName)
{
    FILE * pFile;
    char     buffer [100];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);

    pFile = fopen (fileName , "r");
   if (pFile == NULL)
    {
        perror ("Error opening file");
        exit (1);
    }
   else
   {
    int i  = 0;
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       strcpy(return_buffer + i, buffer);
       i += strlen(buffer);
     }
     fclose (pFile);
   }
   return return_buffer;
}

void GETresponse(Request *request, Response *response, Config *config)
{
    (void )config;
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

// NETWORKING --------------------------------------------------
void init_socket(t_socket *_socket)
{
    // init_socket -------------------------
    _socket->server_fd = 0;
    _socket->new_socket = 0;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(PORT);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    // ------------------------------------
}

void startServer(t_socket *socket)
{
    if (bind(socket->server_fd, (struct sockaddr *)&socket->address, sizeof(socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket->server_fd , 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer(Config *config)
{
    Request     request;
    t_socket    _socket;

    init_socket(&_socket);
    if ((_socket.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    startServer(&_socket);
    while (1)
    {
        ++request;
        if ((_socket.new_socket = accept(_socket.server_fd, (struct sockaddr *)&_socket.address, (socklen_t *)&_socket.addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        _socket.valread = readRequest(_socket.new_socket, &request);
        checkRequest(&request);
        response(_socket.new_socket, request, config);
        close(_socket.new_socket);
    }
    close(_socket.server_fd);

}