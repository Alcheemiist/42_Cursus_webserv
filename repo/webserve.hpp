#ifndef WEBSERVE_HPP
#define WEBSERVE_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include "./config/parse_confile.hpp"

#define PORT 8080


typedef struct s_socket
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen;
    long valread;

} t_socket;

class Config {
    private:
        int port;
        std::string Defaultpath;

    public:
        Config() : port(8080), Defaultpath("/www") { };
        ~Config() {};
        int getPort() { return port; };
        std::string getDefaultpath() { return Defaultpath; };
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

        Socket &operator=(const Socket &other) {
            this->server_fd = other.server_fd;
            this->new_socket = other.new_socket;
            this->address = other.address;
            this->addrlen = other.addrlen;
            this->valread = other.valread;
            return *this;
        };
        Socket(const Socket &other) {
            this->server_fd = other.server_fd;
            this->new_socket = other.new_socket;
            this->address = other.address;
            this->addrlen = other.addrlen;
            this->valread = other.valread;
        };
        int getServer_fd() { return server_fd; };
        int getNew_socket() { return new_socket; };
        struct sockaddr_in getAddress()  { return address;};
        int getAddrlen() { return addrlen; };
        long getValread() { return valread; };

        void setServer_fd(int _server_fd) 
        { 
            if (_server_fd < 0)
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

void LaunchServer(parse_config  *c);


#endif