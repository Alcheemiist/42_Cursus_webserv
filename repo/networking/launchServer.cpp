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
void startServer(Socket *socket)
{
    // startingServer -------------------------
    if (bind(socket->getServer_fd(), (struct sockaddr *)&socket->getAddress(), sizeof(socket->getAddress())) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket->getServer_fd(), 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer(Config *c)
{
    Socket      _socket;
    Request     request;
    (void )c;

    // Creating _socket file descriptor
    _socket.setServer_fd(socket(AF_INET, SOCK_STREAM, 0));
    startServer(&_socket);
    while (1)
    {
        ++request;
        _socket.setNew_socket(accept(_socket.getServer_fd(), (struct sockaddr *)&_socket.getAddress(), (socklen_t *)&_socket.getAddrlen()));
        _socket.setValread(readRequest(_socket.getNew_socket(), &request));
        checkRequest(&request);
        response(_socket.getNew_socket(), request);
        close(_socket.getNew_socket());
    }
    close(_socket.getServer_fd());
}
