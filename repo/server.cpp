#include "webserve.hpp"

int main(int argc, char const *argv[])
{
    Config config;

    if (argc != 2)
    {
        std::cout << "Usage: webserve <config file>\n";
        exit(1);
    }
    if (ParseConfig(argv[1], &config))
    {
        std::cout << "Error parsing config file\n";
        exit(1);
    }
    LaunchServer(&config);
    return 0;
}
