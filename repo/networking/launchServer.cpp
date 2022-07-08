#include "./elements.hpp"
#define SENDER_MAXSIZE 128
#define DATA_MAXSIZE 512
#include <fcntl.h>

#define MAX_CLIENTS 20
t_socket *_socket = new t_socket;
#define NO_SOCKET -1
#define MAX_MESSAGES_BUFFER_SIZE 10

// message --------------------------------------------------------------------
typedef struct
{
    char sender[SENDER_MAXSIZE];
    char data[DATA_MAXSIZE];
} message_t;

// message queue --------------------------------------------------------------
typedef struct
{
    int size;
    message_t *data;
    int current;
} message_queue_t;

int create_message_queue(int queue_size, message_queue_t *queue)
{
    queue->data = (message_t *)calloc(queue_size, sizeof(message_t));
    queue->size = queue_size;
    queue->current = 0;

    return 0;
}

// peer -----------------------------------------------------------------------
typedef struct
{
    int socket;
    struct sockaddr_in addres;

    /* Messages that waiting for send. */
    message_queue_t send_buffer;

    /* Buffered sending message.
     *
     * In case we doesn't send whole message per one call send().
     * And current_sending_byte is a pointer to the part of data that will be send next call.
     */
    message_t sending_buffer;
    size_t current_sending_byte;

    /* The same for the receiving message. */
    message_t receiving_buffer;
    size_t current_receiving_byte;
} peer_t;

peer_t connection_list[MAX_CLIENTS];

int create_peer(peer_t *peer)
{
    create_message_queue((int)MAX_MESSAGES_BUFFER_SIZE, (message_queue_t *)&peer->send_buffer);

    peer->current_sending_byte = -1;
    peer->current_receiving_byte = 0;

    return 0;
}

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

void accepteConnection(t_socket *_socket)
{
    std::cout << "Waiting for connection..." << std::endl;
    if ((_socket->new_socket = accept(_socket->server_fd, (struct sockaddr *)&_socket->address, (socklen_t *)&_socket->addrlen)) < 0)
    {
        perror("In accept");
        exit(EXIT_FAILURE);
    }
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

void init_socket(t_socket *_socket, parse_config *config, int index_port)
{
    (void)config;
    // init_socket -------------------------
    _socket->server_fd = 0;
    _socket->new_socket = 0;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(PORT + index_port);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    // create socket -------------------------
    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
}

int build_fd_sets(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, int listen_sock)
{
    int max_sd = listen_sock;
    FD_ZERO(read_fds);
    FD_ZERO(write_fds);
    FD_ZERO(except_fds);
    FD_SET(listen_sock, read_fds);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (connection_list[i].socket != NO_SOCKET)
        {
            FD_SET(connection_list[i].socket, read_fds);
            FD_SET(connection_list[i].socket, write_fds);
            FD_SET(connection_list[i].socket, except_fds);
            if (connection_list[i].socket > max_sd)
                max_sd = connection_list[i].socket;
        }
    }
    return max_sd;
    // }
    //     int i;

    //     FD_ZERO(read_fds);

    //     FD_SET((_socket + (Config->get_server_vect().size() - 1))->new, read_fds);

    //     FD_SET(listen_sock, read_fds);
    //     for (i = 0; i < MAX_CLIENTS; ++i)
    //         if (connection_list[i].socket != NO_SOCKET)
    //             FD_SET(connection_list[i].socket, read_fds);

    //     FD_ZERO(write_fds);
    //     for (i = 0; i < MAX_CLIENTS; ++i)
    //         if (connection_list[i].socket != NO_SOCKET && connection_list[i].send_buffer.current > 0)
    //             FD_SET(connection_list[i].socket, write_fds);

    //     FD_ZERO(except_fds);
    //     FD_SET(STDIN_FILENO, except_fds);
    //     FD_SET(listen_sock, except_fds);
    //     for (i = 0; i < MAX_CLIENTS; ++i)
    //         if (connection_list[i].socket != NO_SOCKET)
    //             FD_SET(connection_list[i].socket, except_fds);

    return 0;
}

void LaunchServer(parse_config *config)
{
    std::map<int, Request> requests;

    int serv_response = 1;
    bool first = true;

    for (unsigned long index_socket = 0; index_socket < config->get_server_vect().size(); index_socket++)
    {
        init_socket(_socket + index_socket, config, index_socket);
        startServer(_socket + index_socket, config);
        std ::cout << "Server socket init Nº: " << index_socket << std::endl;
    }
    std::cout << std::endl;

    /* Set nonblock for stdin. */
    int flag = fcntl(STDIN_FILENO, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flag);
    // fcntl(fd, F_SETFL, O_NONBLOCK);

    unsigned long i;
    for (i = 0; i < config->get_server_vect().size(); ++i)
    {
        connection_list[i].socket = NO_SOCKET;
        create_peer(&connection_list[i]);
    }

    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;

    int high_sock = (_socket + (config->get_server_vect().size() - 1))->server_fd;
    printf("Waiting for incoming connections.\n");

    while (1)
    {
        high_sock = build_fd_sets(&read_fds, &write_fds, &except_fds, high_sock);
        for (i = 0; i < MAX_CLIENTS; ++i)
        {
            if (connection_list[i].socket > high_sock)
                high_sock = connection_list[i].socket;
        }

        int activity = select(high_sock + 1, &read_fds, &write_fds, &except_fds, NULL);
        (void)activity;

        switch (activity)
        {
        case -1:
            perror("select()");
            shutdown_properly(EXIT_FAILURE);

        case 0:
            // you should never get here
            printf("select() returns 0.\n");
            shutdown_properly(EXIT_FAILURE);

        default:
            /* All set fds should be checked. */
            if (FD_ISSET(STDIN_FILENO, &read_fds))
            {
                if (handle_read_from_stdin() != 0)
                    shutdown_properly(EXIT_FAILURE);
            }

            if (FD_ISSET(listen_sock, &read_fds))
            {
                handle_new_connection();
            }

            if (FD_ISSET(STDIN_FILENO, &except_fds))
            {
                printf("except_fds for stdin.\n");
                shutdown_properly(EXIT_FAILURE);
            }

            if (FD_ISSET(listen_sock, &except_fds))
            {
                printf("Exception listen socket fd.\n");
                shutdown_properly(EXIT_FAILURE);
            }

            for (i = 0; i < MAX_CLIENTS; ++i)
            {
                if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &read_fds))
                {
                    if (receive_from_peer(&connection_list[i], &handle_received_message) != 0)
                    {
                        close_client_connection(&connection_list[i]);
                        continue;
                    }
                }

                if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &write_fds))
                {
                    if (send_to_peer(&connection_list[i]) != 0)
                    {
                        close_client_connection(&connection_list[i]);
                        continue;
                    }
                }

                if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &except_fds))
                {
                    printf("Exception client fd.\n");
                    close_client_connection(&connection_list[i]);
                    continue;
                }
            }
        }

        if (serv_response == 1)
        {

            accepteConnection(_socket);

            serv_response = 2;
        }
        else if (serv_response == 2)
        {

            char *buffer = (char *)malloc(sizeof(char) * BUFER_SIZE);
            size_t bytes = readSocketBuffer(_socket, buffer);

            if (first)
            {
                Request request(buffer, bytes);
                requests.insert(std::pair<int, Request>(_socket->new_socket, request));
                first = false;
            }
            else
                requests[_socket->new_socket].fill_body(buffer, bytes);

            if (requests[_socket->new_socket].getIsComplete())
            {
                serv_response = 3;
            }
        }
        else if (serv_response == 3)
        {
            requests[_socket->new_socket].show();

            response(_socket->new_socket, &requests[_socket->new_socket], config);

            close(_socket->new_socket);
            serv_response = 1;
            first = true;
        }
    }

    close(_socket->server_fd);
}