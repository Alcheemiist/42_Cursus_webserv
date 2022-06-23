#include "./elements.hpp"

// REQUEST --------------------------------------------------

long readRequest(int new_socket, Request *request)
{
    request->setbytes(read(new_socket, request->getbuffer(), 30000));
    printf("%s\n", request->getbuffer());
    return request->getbytes();
}

void checkRequest(Request *request)
{
    (void)request;
}

// RESPONSE --------------------------------------------------
void GETresponse(Request *request, Response *response)
{
    (void)request;

    printf("im doing get response\n");
    printf("%s\n", response->getHello());
}

void POSTresponse()
{
    printf("im doing post response\n");
}

void PUTresponse()
{
    printf("im doing put response\n");
}

void DELETEresponse()
{
    printf("im doing delete response\n");
}

void HEADresponse()
{
    printf("im doing head response\n");
}

void response(int new_socket, Request request)
{
    Response response;

    if (request.getType().compare("GET") == 1)
        GETresponse(&request, &response);
    else if (request.getType().compare("POST") == 0)
        POSTresponse();
    else if (request.getType().compare("DELETE") == 0)
        DELETEresponse();
    else
        HEADresponse();
    send(new_socket, response.getHello(), response.getSize(), MSG_OOB);
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
    memset(_socket->address.sin_zero, '\0', sizeof _socket->address.sin_zero);
    _socket->addrlen = sizeof(_socket->address);
    // ------------------------------------
}

void startServer(t_socket *_socket)
{
    // startingServer -------------------------
    if (bind(_socket->server_fd, (struct sockaddr *)&_socket->address, sizeof(_socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(_socket->server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer()
{
    t_socket _socket;

    Request request;

    init_socket(&_socket);

    // Creating _socket file descriptor
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
        printf("----------- New connection accepted -------------\n");
        if (_socket.new_socket > 0)
            printf("++++++++++++++ Request  ++++++++++++++++\n");
        _socket.valread = readRequest(_socket.new_socket, &request);
        checkRequest(&request);
        printf("\n+++++++++++++ Response ++++++++++++++++++\n");
        response(_socket.new_socket, request);
        close(_socket.new_socket);
        printf("----------- End connection %d -------------\n ** \n", request.getRequestNum());
    }
    close(_socket.server_fd);
}
