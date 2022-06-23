#include "../webserve.hpp"
#include "./socketo.hpp"

char hello[1000] = "HTTP/1.1 200 OK\nAlchemist: is here\n\n<div class=\"hilite-title text-center text-uppercase\">ALCHEMIST</div>";

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

long get_request(int new_socket)
{
    char buffer[30000] = {0};
    long bytes = read(new_socket, buffer, 30000);
    printf("%s\n", buffer);

    return bytes;
}

void response(int new_socket)
{
    printf("%s\n", hello);
    send(new_socket, hello, strlen(hello), MSG_OOB);
}
void LaunchServer()
{
    t_socket _socket;
    int request_num = 0;

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
        request_num++;
        printf("----------- Waiting for new connection -------------\n");

        if ((_socket.new_socket = accept(_socket.server_fd, (struct sockaddr *)&_socket.address, (socklen_t *)&_socket.addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        if (_socket.new_socket > 0)
            printf("++++++++++++++ Request  ++++++++++++++++\n");
        _socket.valread = get_request(_socket.new_socket);

        printf("\n+++++++++++++ Response ++++++++++++++++++\n");
        response(_socket.new_socket);

        close(_socket.new_socket);
        printf("----------- End connection %d -------------\n ** \n", request_num);
    }
    close(_socket.server_fd);
}
