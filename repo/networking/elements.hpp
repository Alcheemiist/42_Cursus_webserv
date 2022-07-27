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

#define MAX_CLIENTS 10000
#define NO_SOCKET -1
#define FALSE 0
#define TRUE 1
#define BUFER_SIZE 1024         //
#define DATA_BUFFER_SIZE 1024   // reading buffer size
#define SENDER_BUFFER_SIZE 1024 // writing buffer size

char *readFile(const char *fileName);
#endif