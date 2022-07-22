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

#define BUFER_SIZE 10000 // reading buffer size

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

int accepteConnection(t_socket *_socket)
{
    t_socket socket_client;

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
    // socket_client = *_socket;
    // socket_client.server_fd = _socket->server_fd;
    return _socket->new_socket;
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
    t_socket _socket_client[1000];
    std::map<int, Request> requests;
    std::map<int, t_socket> sockets;

    unsigned long nServers = config->get_server_vect().size();
    char buffer[1000];
    int *serv_response = new int[MAX_CLIENTS];
    bool *first = new bool[MAX_CLIENTS];
    int index_client = 0;

    int i, len, rc, on = 1, listen_sd, max_sd;
    struct sockaddr_in6 addr;
    struct timeval timeout;

    struct fd_set master_set, working_rd_set, working_wr_set, working_er_set;
    struct fd_set backup_rd_set, backup_wr_set, backup_er_set;

    FD_ZERO(&master_set);
    FD_ZERO(&working_rd_set);
    FD_ZERO(&working_wr_set);
    FD_ZERO(&working_er_set);

    FD_ZERO(&backup_rd_set);
    FD_ZERO(&backup_wr_set);
    FD_ZERO(&backup_er_set);

    timeout.tv_sec = 2 * 60;
    timeout.tv_usec = 0;
    int new_socket = 0;
    int index_request = 0;

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
        sockets.insert(std::pair<int, t_socket>(index_request, _socket_server[i]));
    }

    for (int i = 0; i <= max_sd + 1; i++)
    {
        char str[5] = {0};
        Request request(str, 0);
        requests.insert(std::pair<int, Request>(i, request));
        serv_response[i] = 1;
        first[i] = true;
    }

    for (int index_cycle = 0;;)
    {
        working_rd_set = backup_rd_set;
        working_wr_set = backup_wr_set;
        working_er_set = backup_er_set;

        // for (int i = 0; i < max_sd; i++)
        //     std::cout << "available server[" << i << "]  fd:" << requests.find(i)->first << std::endl;

        std::cout << "Waiting on select()...\n";
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
            // which socket is ready? is for read or write ?
            std::cout << "  select() catch a ready fds for R/W successfuly RC= " << rc << std::endl;
            for (int i = 0; i <= max_sd; i++)
            {
                if (FD_ISSET(i, &working_rd_set))
                {
                    std::cout << "  FD " << _socket_server[i - 3].server_fd << " == " << i << "IS SET READING" << std::endl;
                    if (serv_response[i] == 1) // accepte connection
                    {
                        std::cout << "  accepte connection : " << _socket_server[i - 3].server_fd << std::endl;
                        int fd = accepteConnection(&_socket_server[i - 3]);
                        serv_response[fd] = 1;
                        first[fd] = true;

                        std::cout << "  client accepte connection : " << fd << std::endl;

                        FD_SET(fd, &working_rd_set);
                        FD_SET(fd, &backup_rd_set);

                        if (fd > max_sd)
                            max_sd = fd;

                        index_request++;
                        serv_response[fd]++;
                        std::cout << " status : " << serv_response[i] << " fd_status : " << serv_response[fd] << std::endl;
                    }
                    if (serv_response[i] == 2) // reading the request by BUFFER_SIZE
                    {

                        char buffer[BUFER_SIZE];
                        // size_t bytes = readSocketBuffer(requests.find(i)->first, &buffer);
                        size_t bytes = read(i, buffer, BUFER_SIZE);
                        std::cout << "{" << buffer << "}" << std::endl;
                        // if (first[i])
                        // {
                        //     Request request(buffer, bytes);
                        //     requests.insert(std::pair<int, Request>(index_request, request));
                        //     first[i] = false;
                        //     std::cout << "  read response 2 (reading request) from fd : " << requests.find(i)->first << " == " << i << std::endl;
                        // }
                        // else
                        //     requests.find(i)->second.fill_body(buffer, bytes);

                        // if (requests.find(i)->second.getIsComplete())
                        // {
                        //     serv_response[i] = 3;
                        //     FD_CLR(requests.find(i)->first, &working_rd_set);
                        //     FD_CLR(requests.find(i)->first, &backup_rd_set);

                        //     FD_SET(requests.find(i)->first, &working_wr_set);
                        //     FD_SET(requests.find(i)->first, &backup_wr_set);
                        //     FD_SET(requests.find(i)->first, &working_er_set);
                        //     std::cout << "  reading response is done  " << requests.find(i)->first << " == " << i << std::endl;
                        // }
                        // break;
                    }
                }

                if (FD_ISSET(i, &working_wr_set))
                {
                    std::cout << "  FD " << requests.find(i)->first << " == " << i << "IS SET WRITING" << std::endl;

                    std::cout << "Ready to write response on fd: " << requests.find(i)->first << " == " << i << std::endl;
                    if (serv_response[i] == 3) // sending request
                    {
                        std::cout << "  send response 3 (sending request) to fd : " << requests.find(i)->first << " == " << i << std::endl;
                        requests[i].show();
                        response(requests.find(i)->first, &requests[i], config);

                        FD_CLR(requests.find(i)->first, &working_wr_set);
                        FD_CLR(requests.find(i)->first, &backup_wr_set);
                        requests.erase(requests.find(i)->first);

                        close(requests.find(i)->first);

                        serv_response[i] = 1;
                        first[i] = true;
                    }
                }
            }
        }
        if (index_cycle++ > 50)
            break;
    }

    std::cout << "  Server Broken " << std::endl;
    for (unsigned long i = 0; i < max_sd; i++)
        close((&_socket_server[i])->server_fd);
    // delete[] _socket;
}

/*
    parsing config file     /me
    netwroking              /
    request / upload /      /
    response                /
    cgi                     /
*/

/*

 if (FD_ISSET(i, &working_wr_set))
            {
                std::cout << "Ready to write response on fd: " << _socket_server[i].server_fd <<  " == " << i << std::endl;
            //     if (serv_response[i] == 3) // sending request
            //     {
            //         requests[i][index_request].show();
            //         response((&_socket[i])->new_socket, &requests[i][index_request], config);
            //         close((&_socket[i])->new_socket);
            //         FD_CLR(_socket[i].new_socket, &working_wr_set);
            //         serv_response[i] = 1;
            //         first[i] = true;
            //         index_request++;
            //     }
            // }
            }
            if (FD_ISSET(i, &working_er_set))
                {
                    std::cout << "Select() err on fd :" << i << std::endl;
                }

                */