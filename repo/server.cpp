#include "webserve.hpp"
#include "./config/parse_confile.hpp"


int main(int argc, char  *argv[])
{
    parse_config Config;

    (void )argv;

    parse_main(argc, argv, &Config);
    Config.read_server();




    LaunchServer(&Config);
    return 0;
}