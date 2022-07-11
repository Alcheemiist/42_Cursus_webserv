#include "./elements.hpp"
#define SENDER_MAXSIZE 128
#define DATA_MAXSIZE 512
#include <fcntl.h>

#define MAX_CLIENTS 20
// t_socket *_socket = new t_socket;
#define NO_SOCKET -1
#define MAX_MESSAGES_BUFFER_SIZE 10
int high_sock;
#include <arpa/inet.h>

// TOOLS --------------------------------------------------
char *readFile(const char *fileName)
{
    FILE *pFile;
    char buffer[100];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);

    pFile = fopen(fileName, "r");
    if (pFile == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    else
    {
        int i = 0;
        while (!feof(pFile))
        {
            if (fgets(buffer, 100, pFile) == NULL)
                break;
            strcpy(return_buffer + i, buffer);
            i += strlen(buffer);
        }
        fclose(pFile);
    }
    return return_buffer;
}

// NETWORKING --------------------------------------------------

int accepteConnection(t_socket *_socket)
{
    if ((_socket->new_socket = accept(_socket->server_fd, (struct sockaddr *)&_socket->address, (socklen_t *)&_socket->addrlen)) < 0)
    {
        perror("In accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        char client_ipv4_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &_socket->address.sin_addr, client_ipv4_str, INET_ADDRSTRLEN);
        printf("Incoming connection from %s:%d.\n", client_ipv4_str, _socket->address.sin_port);
    }
    return 0;
}

size_t readSocketBuffer(t_socket *_socket, char *buffer)
{
    // std::cout << "reading request" << std::endl;
    return read(_socket->new_socket, buffer, BUFER_SIZE);
}

void startServer(t_socket *socket, parse_config *config)
{
    (void)config;

    if (bind(socket->server_fd, (struct sockaddr *)&socket->address, sizeof(socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket->server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void init_socket(t_socket *_socket, parse_config *config)
{
    static int i;

    (void)config;
    _socket->server_fd = 0;
    _socket->new_socket = 0;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(PORT + i++);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);

    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer(parse_config *config)
{
    t_socket *__socket = new t_socket;
    std::map<int, Request> requests;

    int serv_response = 1;
    bool first = true;

    for (unsigned long i = 0; i < config->get_server_vect().size(); i++)
    {
        init_socket(__socket + i, config);
        startServer(__socket + i, config);
    }

    // fd_set master_rd_set, working_rd_set; // reading fd sets
    // fd_set master_wr_set, working_wr_set; // writing fd sets

    // int max_fd = 0;

    // FD_ZERO(&master_rd_set);
    // FD_ZERO(&master_wr_set);

    // for (unsigned long i = 0; i < config->get_server_vect().size(); i++)
    // {
    //     int sock = (__socket + i)->server_fd;
    //     FD_SET(sock, &master_rd_set);
    //     if (sock > max_fd)
    //         max_fd = sock;
    // }
    // int select_ret;
    int index = 0;

    while (1)
    {
        if (serv_response == 1) // accepte connection
        {
            accepteConnection(__socket);
            serv_response = 2;
        }
        else if (serv_response == 2) // reading the request
        {
            char *buffer = (char *)malloc(sizeof(char) * BUFER_SIZE);
            size_t bytes = readSocketBuffer(__socket, buffer);

            if (first)
            {
                Request request(buffer, bytes);
                requests.insert(std::pair<int, Request>(index, request));
                first = false;
            }
            else
                requests[index].fill_body(buffer, bytes);

            if (requests[index].getIsComplete())
            {
                serv_response = 3;
            }
        }
        else if (serv_response == 3) // sending request
        {
            requests[index].show();

            response((__socket)->new_socket, &requests[index], config);

            close((__socket)->new_socket);
            serv_response = 1;
            first = true;
            index++;
        }
    }

    for (int i = 0; i < config->get_server_vect().size(); i++)
        close((__socket + i)->server_fd);
}