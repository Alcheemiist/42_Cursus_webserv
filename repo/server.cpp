#include "webserve.hpp"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: webserve <config file>\n");
        exit(1);
    }
    if (ParseConfig(argv[1]))
    {
        printf("Error parsing config file\n");
        exit(1);
    }
    LaunchServer();
    system("leaks a.out");
    return 0;
}
