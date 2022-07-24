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

int main(int argc, char *argv[])
{
    parse_config Config;

    if (argc == 1)
        std::cout << red << " Default Webserve Configuration  " << def << std::endl;
    try 
    {
        if (setup_signals() != 0)
            throw std::runtime_error("setup_signals() failed");
        if (parse_main(argc, argv, &Config) != 0)
            throw std::runtime_error("Error while parsing config file.");

        LaunchServer(&Config);
    }
    catch (std::exception &e)
    {
        std::cout << red << " { " << e.what() << " }" << def << std::endl;
        shutdown_properlyy(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cout << red << " { Unknown error }" << def << std::endl;
        shutdown_properlyy(EXIT_FAILURE);
    }
    return 0;
}