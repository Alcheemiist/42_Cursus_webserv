# ifndef WEBSERVE_HPP
#define WEBSERVE_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

void LaunchServer();
int  ParseConfig(const char *s);

#endif