# ifndef SOCKETO_HPP
#define SOCKETO_HPP

typedef struct s_socket 
{
     int server_fd;
    int  new_socket;            
    struct sockaddr_in address;
    int addrlen;
    long valread;


} t_socket;


#endif 