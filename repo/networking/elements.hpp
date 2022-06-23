#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "../webserve.hpp"

char hello[1000] = "HTTP/1.1 200 OK\nAlchemist: is here\n\n<div class=\"hilite-title text-center text-uppercase\">ALCHEMIST</div>";

typedef struct s_socket
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen;
    long valread;

} t_socket;

class Request
{
private:
    std::string _type;
    char buffer[30000];
    long bytes;
    int request_num;

public:
    Request()
    {
        request_num = 0;
        _type = "";
        bytes = 0;
    };
    Request &operator++()
    {
        ++request_num;
        return *this;
    }

    void setbytes(long bytes)
    {
        this->bytes = bytes;
    }
    long getbytes()
    {
        return bytes;
    }
    void setbuffer(char *buffer)
    {
        strcpy(this->buffer, buffer);
    }
    char *getbuffer()
    {
        return buffer;
    }
    std::string getType() const { return _type; };
    int getRequestNum() const { return request_num; };
};

class Response
{
private:
    char hello[10000];

public:
    Response() { strcpy(hello, "HTTP/1.1 200 OK\nAlchemist: is here\n\n<div class=\"hilite-title text-center text-uppercase\">ALCHEMIST</div>"); };
    const char *getHello() const { return hello; };
    size_t getSize() const { return strlen(hello); };
};

#endif