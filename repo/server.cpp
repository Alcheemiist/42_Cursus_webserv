#include "webserve.hpp"

void shutdown_properlyy(int code)
{
    std::cout << green << "Shutdown server properly." << def << std::endl;
    exit(code);
}

void handle_signal_action(int sig_number)
{
    if (sig_number == SIGINT)
    {
        std::cout << red << " { SIGINT was catched! }" << def << std::endl;
        shutdown_properlyy(EXIT_SUCCESS);
    }
    else if (sig_number == SIGPIPE)
    {
        std::cout << red << " { SIGPIPE was catched! }" << def << std::endl;
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


int main(int ac, char **av, char **ep)
{
    parse_config Config;

    // if (setup_signals() != 0)
    //     exit(EXIT_FAILURE);
    
    parse_main(ac, av, Config);
    
    // LaunchServer(&Config);
    return 0;
}