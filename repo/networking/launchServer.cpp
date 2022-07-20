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
    if (listen(socket->server_fd, 32) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void init_socket(t_socket *_socket, parse_config *config, int i)
{
    (void)config;
    _socket->server_fd = 0;
    _socket->new_socket = 0;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(PORT + i);
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
    t_socket *_socket = new t_socket[MAX_CLIENTS];
    std::map<int, Request> *requests = new std::map<int, Request>[1000];

    int i, len, rc, on = 1;
    int listen_sd, max_sd, new_sd;
    int desc_ready, end_server = 0;
    int close_conn;
    char buffer[800];
    struct sockaddr_in6 addr;
    struct timeval timeout;
    struct fd_set master_set, working_rd_set, working_wr_set;

    unsigned long nServers = 1;
    int *serv_response = new int[nServers];
    bool *first = new bool[nServers];
    int index_request = 0;

    /* ------------- Creating Sockets------------------------- */
    /* Create a stream socket to receive incoming connections on */
    for (unsigned long i = 0; i < nServers; i++)
    {
        serv_response[i] = 1;
        first[i] = true;
    }
    
    for (unsigned long i = 0; i < nServers; i++)
        init_socket(&_socket[i], config, i);
    /* ---------- end creating sockets ----------------------- */

    /* --------- Allow socket descriptor to be reuseable ------ */
    for (unsigned long i = 0; i < nServers; i++)
        if ((rc = setsockopt(_socket[i].server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
        {
            printf("setsockopt(%lu) failed", i);
            close(_socket[i].server_fd);
            exit(-1);
        }
    /* ---------- end allow socket descriptor to be reuseable ------ */

    /* ---------- Set up socket to be non-blocking ----------- */
    /*  Set socket to be nonblocking. All of the sockets for  the incoming connections will also be     */
    /*  nonblocking since they will inherit that state from the listening socket.                       */
    for (unsigned long i = 0; i < nServers; i++)
        if ((fcntl(_socket[i].server_fd, F_SETFL, O_NONBLOCK)) < 0)
        {
            printf("fcntl(%lu) failed", i);
            close(_socket[i].server_fd);
            exit(-1);
        }
    /* ---------- end set socket to be nonblocking ----------- */

    /* -------- Bind the socket  & Set the listen back log ---- */
    for (unsigned long i = 0; i < nServers; i++)
        startServer(&_socket[i], config);

    /*           Initialize the master fd_set                     */
    for (unsigned long i = 0; i < nServers; i++)
    {
        FD_ZERO(&master_set);
        max_sd = _socket[i].server_fd;
        FD_SET(_socket[i].server_fd, &master_set);
    }

    timeout.tv_sec = 3 * 60;
    timeout.tv_usec = 0;

    for (;;)
    {
        /**********************************************************/
        /* Copy the master fd_set over to the working fd_set.     */
        /**********************************************************/

        memcpy(&working_rd_set, &master_set, sizeof(master_set));
        memcpy(&working_wr_set, &master_set, sizeof(master_set));
        max_sd = _socket[0].server_fd;

        /**********************************************************/

        std::cout <<"Waiting on select()...\n"   << std::endl;
        rc = select(max_sd + 1, &working_rd_set, &working_wr_set, NULL, &timeout);

        if (rc < 0)
        {
            std::cout << "  select() failed" << std::endl;
            break;
        }
        else if (rc == 0)
        {
            std::cout << "  select() timed out.\n" << std::endl;
            break;
        }
        else
        {
            // which socket is ready? is for read or write ?
            std::cout <<"  select() was successful" << rc << std::endl;
            break;
            /*nServers = 1;
            for (unsigned long i = 0; i < nServers; i++)
            {
                if (serv_response[i] == 1) // accepte connection
                {
                    accepteConnection(&_socket[i]);
                    serv_response[i] = 2;
                }
                else if (serv_response[i] == 2) // reading the request by BUFFER_SIZE
                {
                    char *buffer = (char *)malloc(sizeof(buffer) * BUFER_SIZE + 1);
                    size_t bytes = readSocketBuffer(&_socket[i], buffer);
                    if (first[i])
                    {
                        Request request(buffer, bytes);
                        requests[i].insert(std::pair<int, Request>(index_request, request));
                        first[i] = false;
                    }
                    else
                        requests[i][index_request].fill_body(buffer, bytes);

                    if (requests[i][index_request].getIsComplete())
                        serv_response[i] = 3;
                }

                
                else if (serv_response[i] == 3) // sending request
                {
                    requests[i][index_request].show();
                    response((&_socket[i])->new_socket, &requests[i][index_request], config);
                    close((&_socket[i])->new_socket);
                    serv_response[i] = 1;
                    first[i] = true;
                    index_request++;
                }
            }*/
        }
    }

    std::cout << "  Server Broken " << std::endl;
    for (unsigned long i = 0; i < nServers; i++)
        close((&_socket[i])->server_fd);
    delete[] _socket;
}

/*
    parsing config file     /me
    netwroking              /
    request / upload /      / 
    response                /
    cgi                     /
*/
