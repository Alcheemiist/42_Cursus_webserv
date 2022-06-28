
#include "../elements.hpp"


// REQUEST --------------------------------------------------

long readRequest(int new_socket, Request *request)
{
    char *buffer = (char *)malloc(sizeof(char) * 30000000);

    Color::Modifier red(Color::FG_RED);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier B_red(Color::BG_RED);

    // Read the request from the client
    request->setbytes( recv(new_socket, buffer, 3000000 , 0) );
    
    // Print the request
    std::cout << red << "\n----(Request*" << request->getRequestNum() << ")------- (N Bytes: " << request->getbytes() << ")------------ "<< def << std::endl;
    std::cout << buffer << std::endl;
    std::cout << red << "-------------------------------------------------" << def << std::endl;
    
    // Parse the request & fill the request object

    // Print the request class attributes

    // Free local buffers
    free(buffer);

    return request->getbytes();
}

void checkRequest(Request *request)
{
    (void)request;
}