#include "./elements.hpp"

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

void close_fds(t_socket *_socket_server, int nServers, std::map<int, t_socket> clients)
{
    for (int i = 0; i < nServers; i++)
        close(_socket_server[i].server_fd);

    for (size_t index_client = 0; index_client < clients.size(); index_client++)
    {
        if (clients[index_client].server_fd)
            close(clients[index_client].server_fd);
        if (clients[index_client].new_socket)
            close(clients[index_client].new_socket);
    }
}

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

void init_socket(t_socket *_socket)
{
    int on = 1, rc = 0;

    _socket->server_fd = 0;
    _socket->new_socket = -1;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(_socket->port);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    if ((rc = setsockopt(_socket->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
    {
        std::cout << red << "setsockopt( ) failed" << def << std::endl;
        close(_socket->server_fd);
        exit(-1);
    }
    if ((fcntl(_socket->server_fd, F_SETFL, O_NONBLOCK)) < 0)
    {
        std::cout << red << "fcntl( ) failed" << def << std::endl;
        close(_socket->server_fd);
        exit(-1);
    }
    if (bind(_socket->server_fd, (struct sockaddr *)&_socket->address, sizeof(_socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(_socket->server_fd, 32) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

// #define PRINT_VALUE(x) println(#x, " = ", x)
// #define PRINT_LINE_VALUE(x) println(__FILE__, ":", __LINE__, " ", #x, " = ", x)
// #include "../config/print.hpp"

void LaunchServer(parse_config *config)
{
    // std::vector<server> servs = config->get_server_vect();
    // for (std::vector<server>::iterator it = servs.begin(); it != servs.end(); it++) {
    // 	PRINT_LINE_VALUE(it->get_listen_port());
    // }
    std::map<int, Request> requests;
    std::map<int, t_socket> clients;
    struct timeval timeout;
    int nServers = config->get_server_vect().size();
    t_socket _socket_server[nServers];
    int *serv_response = new int[MAX_CLIENTS];
    bool *first = new bool[MAX_CLIENTS];
    int rc, max_sd, index_client;
    struct fd_set working_rd_set, working_wr_set, working_er_set;
    struct fd_set backup_rd_set, backup_wr_set, backup_er_set;

    rc = max_sd = index_client = 0;
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;

    FD_ZERO(&working_rd_set);
    FD_ZERO(&working_wr_set);
    FD_ZERO(&working_er_set);
    FD_ZERO(&backup_rd_set);
    FD_ZERO(&backup_wr_set);
    FD_ZERO(&backup_er_set);

    std::cout << "Launching " << nServers << " server..." << std::endl;
    for (int i = 0; i < nServers; i++)
    {
        std::cout << green << "init server " << config->get_server_vect()[i].get_name(0) << " on port: " << config->get_server_vect()[i].get_listen_port()
                  << " path root :" << config->get_server_vect()[i].get_root() << def << std::endl;
        serv_response[i] = 1;
        first[i] = true;
        _socket_server[i].port = config->get_server_vect()[i].get_listen_port();
        init_socket(&_socket_server[i]);
        FD_SET(_socket_server[i].server_fd, &backup_rd_set);
        if (_socket_server[i].server_fd > max_sd)
            max_sd = _socket_server[i].server_fd;
    }

    for (int index_cycle = 0;;)
    {
        memcpy(&working_rd_set, &backup_rd_set, sizeof(backup_rd_set));
        memcpy(&working_wr_set, &backup_wr_set, sizeof(backup_wr_set));

        std::cout << "\nWaiting on select(" << index_cycle << ")...\n";
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
                    clients[index_client].index_server = i;

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
                    int bytes = -1;
                    int fd = clients[i].server_fd;

                    char buffer[BUFER_SIZE + 1];
                    if ((bytes = read(fd, buffer, BUFER_SIZE)) < 0)
                        continue;
                    buffer[bytes] = '\0';
                    if (first[i])
                    {
                        Request request((buffer), bytes, clients[i].server_fd);
                        request.set_request_num(index_cycle);
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
                    // free(/buffer);
                }

                if (FD_ISSET(clients[i].server_fd, &working_wr_set) && serv_response[i] == 3)
                {
                    std::cout << "  ready to responde the client  " << requests.find(clients[i].server_fd)->first << std::endl;

                    if (serv_response[i] == 3) // sending request
                    {
                        std::cout << "  send response 3 (sending request) to fd : " << requests.find(clients[i].server_fd)->first << " == " << clients[i].server_fd << std::endl;

                        response(requests.find(clients[i].server_fd)->first, &requests.find(clients[i].server_fd)->second, config, clients[i].index_server);

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

    close_fds(_socket_server, nServers, clients);
    std::cout << green << "Shutdown Server Properly." << def << std::endl;
}