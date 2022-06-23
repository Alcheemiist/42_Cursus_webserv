#ifndef WEBSERVE_HPP
#define WEBSERVE_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include <iostream>

#define PORT 8080

void LaunchServer();
int ParseConfig(const std::string s);

#endif