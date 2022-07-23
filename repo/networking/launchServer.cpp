#include "./elements.hpp"
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define MAX_CLIENTS 20
#define NO_SOCKET -1
#define MAX_MESSAGES_BUFFER_SIZE 10
#define SENDER_MAXSIZE 128
#define DATA_MAXSIZE 512

#define FALSE 0
#define TRUE 1

#define BUFER_SIZE 10024 // reading buffer size

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
        return NULL;
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

t_socket accepteConnection(t_socket *_socket)
{
    t_socket __socket;

    if ((__socket.server_fd = accept(_socket->server_fd, (struct sockaddr *)&_socket->address, (socklen_t *)&_socket->addrlen)) < 0)
    {
        perror("In accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Incoming connection from %d.\n", __socket.server_fd);
    }
    return __socket;
}

size_t readSocketBuffer(int fd, char **buffer)
{
    std::cout << "reading request" << std::endl;
    return read(fd, &buffer, BUFER_SIZE);
}

void startServer(t_socket *socket, parse_config *config)
{
    (void)config;

    if (bind(socket->server_fd, (struct sockaddr *)&socket->address, sizeof(socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket->server_fd, 32) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void init_socket(t_socket *_socket, int port)
{
    _socket->server_fd = 0;
    _socket->new_socket = -1;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(port);
    // memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer(parse_config *config)
{
    t_socket _socket_server[MAX_CLIENTS];

    std::map<int, Request> requests;
    std::map<int, t_socket> clients;

    unsigned long nServers = 2;
    char buffer[1000];
    int *serv_response = new int[1000], rc, on = 1, max_sd, index_client = 0;
    bool *first = new bool[1000];

    struct timeval timeout;
    struct fd_set working_rd_set, working_wr_set, working_er_set;
    struct fd_set backup_rd_set, backup_wr_set, backup_er_set;

    FD_ZERO(&working_rd_set);
    FD_ZERO(&working_wr_set);
    FD_ZERO(&working_er_set);

    FD_ZERO(&backup_rd_set);
    FD_ZERO(&backup_wr_set);
    FD_ZERO(&backup_er_set);

    timeout.tv_sec = 2 * 60;
    timeout.tv_usec = 0;

    for (unsigned long i = 0; i < nServers; i++)
    {
        serv_response[i] = 1;
        first[i] = true;

        init_socket(&_socket_server[i], PORT + i);

        if ((rc = setsockopt(_socket_server[i].server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
        {
            printf("setsockopt(%lu) failed", i);
            close(_socket_server[i].server_fd);
            exit(-1);
        }
        if ((fcntl(_socket_server[i].server_fd, F_SETFL, O_NONBLOCK)) < 0)
        {
            printf("fcntl(%lu) failed", i);
            close(_socket_server[i].server_fd);
            exit(-1);
        }
        startServer(&_socket_server[i], config);

        FD_SET(_socket_server[i].server_fd, &backup_rd_set);

        if (_socket_server[i].server_fd > max_sd)
            max_sd = _socket_server[i].server_fd;
    }

    for (int index_cycle = 0;;)
    {
        memcpy(&working_rd_set, &backup_rd_set, sizeof(backup_rd_set));
        memcpy(&working_wr_set, &backup_wr_set, sizeof(backup_wr_set));

        std::cout << "\nWaiting on select()...\n";
        rc = select(max_sd + 1, &working_rd_set, &working_wr_set, &working_er_set, &timeout);

        if (rc < 0)
        {
            std::cout << "  select() failed" << std::endl;
            break;
        }
        else if (rc == 0)
        {
            std::cout << "  select() timed out.\n"
                      << std::endl;
            break;
        }
        else
        {
            std::cout << "  There is a ready fds for R/W successfuly RC= " << rc << std::endl;

            // only for servers to accepte new connections
            for (int i = 0; i < nServers; i++)
                if (FD_ISSET(_socket_server[i].server_fd, &working_rd_set))
                {
                    std::cout << "  accepte connection from server_socket : " << _socket_server[i].server_fd << std::endl;
                    t_socket _server = accepteConnection(&_socket_server[i]);
                    clients[index_client] = _server;
                    clients[index_client].new_socket = _socket_server[i].server_fd;
                    std::cout << "  client accepte connection : " << clients[index_client].server_fd << std::endl;

                    serv_response[index_client] = 1;
                    first[index_client] = true;

                    FD_SET(clients[index_client].server_fd, &working_rd_set);
                    FD_SET(clients[index_client].server_fd, &backup_rd_set);

                    if (clients[index_client].server_fd > max_sd)
                        max_sd = clients[index_client].server_fd;

                    serv_response[index_client] = 2;
                    std::cout << " status client : " << serv_response[index_client] << " max_sd : " << max_sd << std::endl;
                    index_client++;
                }

            // only for clients
            for (int i = 0; i < index_client; i++)
            {
                if (FD_ISSET(clients[i].server_fd, &working_rd_set) && serv_response[i] == 2)
                {
                    std::cout << " ready to read from clients.server.fd " << clients[i].server_fd << " accepted from server.fd " << clients[i].new_socket << std::endl;
                    char *buffer = (char *)malloc(sizeof(char *) * BUFER_SIZE + 1);
                    int bytes = -1;
                    int fd = clients[i].server_fd;
                    if ((bytes = read(fd, buffer, 10024)) < 0)
                        continue;
                    if (first[i])
                    {
                        Request request((buffer), bytes, clients[i].server_fd);
                        requests.insert(std::pair<int, Request>(clients[i].server_fd, request));
                        first[i] = false;

                        std::cout << " created request->first : " << requests.find(clients[i].server_fd)->first
                                  << " == client.server_fd " << clients[i].server_fd << std::endl;
                    }
                    else
                        requests.find(clients[i].server_fd)->second.fill_body(buffer, bytes);

                    if (requests.find(clients[i].server_fd)->second.getIsComplete())
                    {
                        std::cout << " request is complete " << std::endl;
                        serv_response[i] = 3;

                        FD_CLR(requests.find(clients[i].server_fd)->first, &working_rd_set);
                        FD_CLR(requests.find(clients[i].server_fd)->first, &backup_rd_set);

                        FD_SET(requests.find(clients[i].server_fd)->first, &working_wr_set);
                        FD_SET(requests.find(clients[i].server_fd)->first, &backup_wr_set);

                        requests.find(clients[i].server_fd)->second.show();
                    }
                    free(buffer);
                }

                if (FD_ISSET(clients[i].server_fd, &working_wr_set))
                {
                    std::cout << "  ready to responde the client  " << requests.find(clients[i].server_fd)->first << std::endl;

                    if (serv_response[i] == 3) // sending request
                    {
                        std::cout << "  send response 3 (sending request) to fd : " << requests.find(clients[i].server_fd)->first << " == " << clients[i].server_fd << std::endl;
                        response(requests.find(clients[i].server_fd)->first, &requests.find(clients[i].server_fd)->second, config);

                        FD_CLR(requests.find(clients[i].server_fd)->first, &working_wr_set);
                        FD_CLR(requests.find(clients[i].server_fd)->first, &backup_wr_set);

                        close(requests.find(clients[i].server_fd)->first);
                        requests.erase(requests.find(clients[i].server_fd)->first);

                        serv_response[i] = 1;
                        first[i] = true;
                    }
                }
            }
        }
    }

    std::cout << "  Server Broken " << std::endl;
    for (unsigned long i = 0; i < max_sd; i++)
        close((&_socket_server[i])->server_fd);

    for (int index_client = 0; index_client < clients.size(); index_client++)
    {
        if (clients[index_client].server_fd)
            close(clients[index_client].server_fd);
        if (clients[index_client].new_socket)
            close(clients[index_client].new_socket);
    }
}