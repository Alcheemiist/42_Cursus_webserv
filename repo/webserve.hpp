#ifndef WEBSERVE_HPP
#define WEBSERVE_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

#define PORT 8080

class Config {
    private:
        int port;
    public:
        Config() : port(8080) { };
        ~Config() {};
        int getPort() { return port; };
};

class Socket {
    private:
        int server_fd;
        int new_socket;
        struct sockaddr_in address;
        int addrlen;
        long valread;
    public:
        Socket() 
        {
            server_fd = 0;
            new_socket = 0;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(PORT);
            memset(address.sin_zero, '\0', sizeof(address.sin_zero));
            addrlen = sizeof(address);
        };
        ~Socket() {};
        int getServer_fd() { return server_fd; };
        int getNew_socket() { return new_socket; };
        struct sockaddr_in getAddress() { return address; };
        int getAddrlen() { return addrlen; };
        long getValread() { return valread; };

        void setServer_fd(int _server_fd) 
        { 
            if (!_server_fd)
            {
                perror("In socket");
                exit(EXIT_FAILURE);
            }
            server_fd = _server_fd;
        };
        void setNew_socket(int _new_socket) 
        {
            if (_new_socket < 0)
            {
                perror("In accept");
                exit(EXIT_FAILURE);
            }
            new_socket = _new_socket;
        };
        void setAddress(struct sockaddr_in _address) { address = _address; };
        void setAddrlen(int _addrlen) { addrlen = _addrlen; };
        void setValread(long _valread) { valread = _valread; };

};

void LaunchServer(Config *c);
int ParseConfig(const std::string s, Config *c);

#endif