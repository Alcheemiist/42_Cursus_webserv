#include "./elements.hpp"

char *readFile(const char *fileName)
{
    char buffer[DATA_BUFFER_SIZE];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);
    int i_size = getFileSize(fileName);
    int fd = open(fileName, O_RDWR);

    if (fd < 0)
        throw std::runtime_error("Error opening file");
    else
        for (int i = 0; i < i_size; i += DATA_BUFFER_SIZE)
        {
            int n = read(fd, buffer, DATA_BUFFER_SIZE);
            for (int i = 0; i < n; i++)
                return_buffer[i] = buffer[i];
            return_buffer[n] = '\0';
            if (n < 0)
                throw std::runtime_error("Error reading file");
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
        throw std::runtime_error("Error accepting connection");
    return __socket;
}

size_t readSocketBuffer(int fd, char **buffer)
{
    return read(fd, &buffer, BUFER_SIZE);
}
