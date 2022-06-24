#include "./elements.hpp"

// REQUEST --------------------------------------------------

long readRequest(int new_socket, Request *request)
{
    request->setbytes(read(new_socket, request->getbuffer(), 30000));
    std::cout << request->getbuffer() << std::endl;
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

    std::cout << "Method " << request->getMethod() << std::endl;
    std::cout << "path " << request->getPath() << std::endl;
    std::cout << "Version " << request->getVersion() << std::endl;
    std::cout << "Host " << request->getHost() << std::endl;
    std::cout << "Connection " << request->getConnection() << std::endl;

    std::cout << response->getHello() << std::endl;
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

void response(int new_socket, Request request)
{
    Response response;

    std::cout << "----> " << request.getMethod() << "\n ";

    if (request.isGoodrequest())
        ERRORresponse(&request, &response);

    if (!(request.getMethod().compare("GET")))
        GETresponse(&request, &response);
    else if (request.getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request.getMethod().compare("DELETE") == 0)
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
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
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

        _socket.valread = readRequest(_socket.new_socket, &request);
        checkRequest(&request);

        response(_socket.new_socket, request);
        close(_socket.new_socket);
    }
    close(_socket.server_fd);
}
