#include "./elements.hpp"

// TOOLS --------------------------------------------------
char *readFile(const char * fileName)
{
    FILE * pFile;
    char     buffer [100];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);

    pFile = fopen (fileName , "r");
   if (pFile == NULL)
    {
        perror ("Error opening file");
        exit (1);
    }
   else
   {
    int i  = 0;
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       strcpy(return_buffer + i, buffer);
       i += strlen(buffer);
     }
     fclose (pFile);
   }
   return return_buffer;
}

// NETWORKING --------------------------------------------------

void init_socket(t_socket *_socket)
{
    // init_socket -------------------------
    _socket->server_fd = 0;
    _socket->new_socket = 0;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = INADDR_ANY;
    _socket->address.sin_port = htons(PORT);
    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    // ------------------------------------
}

void startServer(t_socket *socket)
{
    if (bind(socket->server_fd, (struct sockaddr *)&socket->address, sizeof(socket->address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket->server_fd , 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void LaunchServer(Config *config)
{
    t_socket    _socket;
    int        serv_response = 1;
    bool        first = true;
    std::map<int, Request> requests;

    init_socket(&_socket);
    if ((_socket.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    startServer(&_socket);
    
    while (1)
    {
        // ++request;
        if (serv_response == 1)
        {
            std::cout << "Waiting for connection..." << std::endl;
            if ((_socket.new_socket = accept(_socket.server_fd, (struct sockaddr *)&_socket.address, (socklen_t *)&_socket.addrlen)) < 0)
            {
                perror("In accept");
                exit(EXIT_FAILURE);
            }
            serv_response = 2;
        }
        else if (serv_response == 2)
        {
            std::cout << "reading request" << std::endl;
            // _socket.valread = readRequest(_socket.new_socket, &request);


            char *buffer = (char *)malloc(sizeof(char) * 30000);
            size_t bytes = read(_socket.new_socket, buffer, 30000);
            
            if (first)
            {
                std::cout << "first" << std::endl;
                Request request(buffer, bytes);
                requests.insert(std::pair<int, Request>(1, request));
                first = false;
            }
            else
                requests[1].fill_body(buffer, bytes);
            if (requests[1].getIsComplete())
                serv_response = 3;
        }
        else if (serv_response == 3)
        {
            std::cout << "done" << std::endl;
            response(_socket.new_socket, requests[1], config);
            close(_socket.new_socket);
            serv_response = 1;
        }
    }
    close(_socket.server_fd);

}