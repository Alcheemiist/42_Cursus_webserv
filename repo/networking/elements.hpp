#pragma once

#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "../webserve.hpp"
#include "./request/Request.hpp"
#include "./response/Response.hpp"
#include "../config/parse_confile.hpp"

#include <sstream>
#include <fcntl.h>
#include <vector>
#include <sys/stat.h>

#define MAX_CLIENTS 100000
#define NO_SOCKET -1
#define FALSE 0
#define TRUE 1
#define BUFER_SIZE 1024         //
#define DATA_BUFFER_SIZE 1024   // reading buffer size
#define SENDER_BUFFER_SIZE 1024 // writing buffer size

char *readFile(const char *fileName);
size_t getFileSize(const char *fileName);
size_t readSocketBuffer(int fd, char **buffer);
t_socket accepteConnection(t_socket *_socket);
void close_fds(t_socket *_socket_server, int nServers, std::map<int, t_socket> clients);
void init_socket(t_socket *_socket);
void LaunchServer(ParseConfig *config);

#endif