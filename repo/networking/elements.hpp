#pragma once

#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include <sstream>
#include <fcntl.h>
#include <vector>
#include <sys/stat.h>

#include "../webserve.hpp"
#include "./request/Request.hpp"
#include "./response/Response.hpp"
#include "../config/parse_confile.hpp"

#define MAX_CLIENTS 10000
#define NO_SOCKET -1
#define MAX_MESSAGES_BUFFER_SIZE 10
#define SENDER_MAXSIZE 128
#define DATA_MAXSIZE 512
#define FALSE 0
#define TRUE 1
#define BUFER_SIZE 4024 // reading buffer size
#define PORT 8080

char *readFile(const char *fileName);

#endif