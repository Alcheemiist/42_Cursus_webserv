#include "webserve.hpp"
#include "./config/parse_confile.hpp"
#include <signal.h>

void shutdown_properlyy(int code)
{
    printf("Shutdown server properly.\n");
    exit(code);
}

void handle_signal_action(int sig_number)
{
    if (sig_number == SIGINT)
    {
        printf("SIGINT was catched!\n");
        shutdown_properlyy(EXIT_SUCCESS);
    }
    else if (sig_number == SIGPIPE)
    {
        printf("SIGPIPE was catched!\n");
        shutdown_properlyy(EXIT_SUCCESS);
    }
}

int setup_signals()
{
    struct sigaction sa;
    sa.sa_handler = handle_signal_action;
    if (sigaction(SIGINT, &sa, 0) != 0)
    {
        perror("sigaction()");
        return -1;
    }
    if (sigaction(SIGPIPE, &sa, 0) != 0)
    {
        perror("sigaction()");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    parse_config Config;

    (void)argv;

    // if (setup_signals() != 0)
    //     exit(EXIT_FAILURE);

    parse_main(argc, argv, &Config);
    // Config.read_server();

    LaunchServer(&Config);
    return 0;
}