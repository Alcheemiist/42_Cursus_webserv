#include "webserve.hpp"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: webserve <config file>\n";
        exit(1);
    }
    if (ParseConfig(argv[1]))
    {
        std::cout << "Error parsing config file\n";
        exit(1);
    }
    LaunchServer();
    return 0;
}
