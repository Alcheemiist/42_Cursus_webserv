#include "../elements.hpp"

void init_socket(t_socket *_socket)
{
    int on = 1;

    _socket->server_fd = 0;
    _socket->new_socket = -1;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(_socket->port);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        throw std::runtime_error("socket failed");
    if ((setsockopt(_socket->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
    {
        close(_socket->server_fd);
        throw std::runtime_error("setsockopt failed");
    }
    if ((fcntl(_socket->server_fd, F_SETFL, O_NONBLOCK)) < 0)
    {
        close(_socket->server_fd);
        throw std::runtime_error("fcntl failed");
    }
    if (bind(_socket->server_fd, (struct sockaddr *)&_socket->address, sizeof(_socket->address)) < 0)
        throw std::runtime_error("bind failed");
    if (listen(_socket->server_fd, 13) < 0)
        throw std::runtime_error("listen failed");
}

void LaunchServer(ParseConfig *config)
{
    std::map<int, Request> requests;
    std::map<int, t_socket> clients;
    std::map<int, Response> responses;

    int nServers = config->get_server_vect().size();
    t_socket _socket_server[nServers];

    int *serv_response = new int[MAX_CLIENTS];
    bool *first = new bool[MAX_CLIENTS];

    int rc, max_sd, index_client;
    struct fd_set working_rd_set, working_wr_set, working_er_set;
    struct fd_set backup_rd_set, backup_wr_set, backup_er_set;
    struct timeval timeout;

    timeout.tv_sec  = 60000;
    timeout.tv_usec = rc = max_sd = index_client = 0;

    FD_ZERO(&working_rd_set);
    FD_ZERO(&working_wr_set);
    FD_ZERO(&working_er_set);
    FD_ZERO(&backup_rd_set);
    FD_ZERO(&backup_wr_set);
    FD_ZERO(&backup_er_set);

    std::cout << "Launching " << nServers << " server..." << std::endl;
    for (int  i = 0; i < nServers; ++i)
    {
        std::cout << green << "init server " << config->get_server_vect()[i].get_name(0) << " on port: " << config->get_server_vect()[i].get_listen_port() << " path root :" << config->get_server_vect()[i].get_root() << def << std::endl;
        serv_response[i] = 1;
        first[i] = true;
        _socket_server[i].port = config->get_server_vect()[i].get_listen_port();
        init_socket(&_socket_server[i]);
        FD_SET(_socket_server[i].server_fd, &backup_rd_set);
        if (_socket_server[i].server_fd > max_sd)
            max_sd = _socket_server[i].server_fd;
    }

    ssize_t size_send = 0;
    std::string str_to_send;
    size_t   *_send_size = new size_t[MAX_CLIENTS];

    for (int index_cycle = 0;;index_cycle++)
    {
        memcpy(&working_rd_set, &backup_rd_set, sizeof(backup_rd_set));
        memcpy(&working_wr_set, &backup_wr_set, sizeof(backup_wr_set));
        
        std::cout << green << "Waiting (" << index_cycle << ")..."  << def << std::endl;
        rc = select(max_sd + 1, &working_rd_set, &working_wr_set, &working_er_set, &timeout);
        
        if (rc < 0)
            throw std::runtime_error("  select() failed");
        else if (rc == 0)
            throw std::runtime_error("Timeout");
        else
        {
            // only for servers to accepte new connections
            for (int i = 0; i < nServers; i++)
                if (FD_ISSET(_socket_server[i].server_fd, &working_rd_set))
                {
                    /* FLAG */serv_response[index_client] = 1;
                    t_socket _server = accepteConnection(&_socket_server[i]);
                    clients[index_client] = _server;
                    clients[index_client].new_socket = _socket_server[i].server_fd;
                    clients[index_client].index_server = i;
                    first[index_client] = true;

                    FD_SET(clients[index_client].server_fd, &working_rd_set);
                    FD_SET(clients[index_client].server_fd, &backup_rd_set);
                    
                    if (clients[index_client].server_fd > max_sd)
                        max_sd = clients[index_client].server_fd;
                    serv_response[index_client]++;
                    index_client++;
                }
            
            // only for clients
            for (int i = 0; i < index_client; i++)
            {

                // request
                if (FD_ISSET(clients[i].server_fd, &working_rd_set) && serv_response[i] == 2)
                {
                    int bytes = -1;
                    int fd = clients[i].server_fd;
                    char buffer[BUFER_SIZE + 1];

                    if ((bytes = read(fd, buffer, BUFER_SIZE)) < 0)
                        continue;
                    buffer[bytes] = '\0';
                    
                    if (first[i])
                    {
                        Request request((buffer), bytes, clients[i].server_fd);
                        request.set_client_addr(clients[i].address);
                        requests.insert(std::pair<int, Request>(clients[i].server_fd, request));
                        first[i] = false;
                    }
                    else
                        requests.find(clients[i].server_fd)->second.fill_body(buffer, bytes);
                        
                    if (requests.find(clients[i].server_fd)->second.getIsComplete())
                    {
                        serv_response[i]++;
                        FD_CLR(requests.find(clients[i].server_fd)->first, &working_rd_set);
                        FD_CLR(requests.find(clients[i].server_fd)->first, &backup_rd_set);
                        FD_SET(requests.find(clients[i].server_fd)->first, &working_wr_set);
                        FD_SET(requests.find(clients[i].server_fd)->first, &backup_wr_set);
                        requests.find(clients[i].server_fd)->second.show();
                    }
                }

                // response
                if (FD_ISSET(clients[i].server_fd, &working_wr_set) && serv_response[i] >= 3)
                {
                    if (serv_response[i] == 3) 
                    {
                        // kansift only headers for now
                        responses.insert(std::pair<int, Response>(i,\
                        
                        // PROCCES RESPONSE 
                        response(&requests.find(clients[i].server_fd)->second, config, clients[i].index_server)));
                        //
                        
                        std::string header = responses[i].getHeader();
                        // show headers 
                        responses[i].show();
                        size_send = write(requests.find(clients[i].server_fd)->first, header.c_str() , header.size());
                        _send_size[i] = size_send;
                        serv_response[i]++;                            
                    }
                    if (serv_response[i] == 4) 
                    {
                        // kansift hna l body by buffer 
                        if (responses[i].getpath().size() > 0 && !responses[i].get_finish())
                        {
                            std::vector<char> buffer = responses[i].get_buffer();
                            size_send = write(requests.find(clients[i].server_fd)->first, buffer.data(), buffer.size());
                            _send_size[i] += size_send;
                            if (size_send < 0)
                                throw std::runtime_error(" error sending buffer");
                        }
                        else if (responses[i].get_finish())
                        {
                            
                            serv_response[i]++;
                            std::cout << green << " **********        RESPONSIYA      ****************** " << def << std::endl;
                            responses[i].show();
                            std::cout << B_def <<  red << " [HEADER_SIZE] : " << blue  << (responses[i].get_header().size()) << def << std::endl;
                            std::cout << B_def <<  red << " [BODY_SIZE] : " << blue  << getFileSize(responses[i].getpath().c_str()) << def << std::endl;
                            std::cout << B_def <<  red << " [FULL SIZE OF RESPONSE] : " << blue  << _send_size[i] << def << std::endl;
                            std::cout << green <<" **********    END RESPONSIYA      ****************** " <<  def << std::endl;
                        }
                    }
                    if (serv_response[i] == 5) 
                    {
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
        if (index_cycle > 1000)
            break;
    }
    close_fds(_socket_server, nServers, clients);
    std::cout << green << "Shutdown Server Properly." << def << std::endl;
}