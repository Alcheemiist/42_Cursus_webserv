 // Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // response 
    char hello[1000];
    strcpy(hello , "HTTP/1.1 200 OK\nAlchemist: is here\n\n \
        <div class=\"hilite-title text-center text-uppercase\"> \
                ALCHEMIST \
        </div> \
     ");

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    int request_num = 0;
    while(1)
    {
        request_num++;
        printf("\n+++++++++++++++++ Waiting for new connection ++++++++++++++++++\n\n");

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))< 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        } 

        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        
        printf("\n+++++++ Request #%d ++++++++\n\n", request_num);
        printf("%s\n",buffer );

        printf("------------------response message sent-------------------");
        // write(new_socket , hello , strlen(hello));
        send(new_socket, hello, strlen(hello), MSG_OOB);

        close(new_socket);
    }
        close(server_fd);
        system("leaks a.out");
        return 0;
   }

   