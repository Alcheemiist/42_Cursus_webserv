#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>

char *readAllFile(char *path)
{
    char buffer[DATA_BUFFER_SIZE];
    char *return_buffer = (char *)malloc(sizeof(char) * 30000000);
    int max_size = getFileSize(path);
    int fd = open(path, O_RDWR);
    size_t read_size = 0;
    
    if (fd < 0)
        throw std::runtime_error("Error opening file");
    else
        for (int i = 0; i < max_size; i += DATA_BUFFER_SIZE)
        {
            int size = read(fd, buffer, DATA_BUFFER_SIZE);
            for (int i = 0; i < size; i++)
                return_buffer[i + read_size] = buffer[i];
            if (size == 0)
                break;
            if (size < 0)
                throw std::runtime_error("Error reading file");
            read_size += size;
        }
        return_buffer[max_size] = '\0';
    return return_buffer;
}

size_t _getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

void ERRORresponse(Request *request, Response *response)
{
    (void)request;
    (void)response;
    std::cout << B_red << "im doing error response status= " << request->getRequestStatus() << B_def << std::endl;
}

void DELETEresponse()
{
    std::cout << "im doing delete response\n";
}

void POSTresponse()
{
    std::cout << "im doing post response\n";
}

std::vector<char> read_by_vector(char *path, Response *response)
{
    int fd = open(path, O_RDWR), n = -1;
    char *_buffer = (char *)malloc(sizeof(char) * (response->getbody_file_size() + 1));
    std::vector<char> buffer;

    if (fd < 0)
        perror("Error opening file");
    else
    {
        for (int i = 0; i < response->getbody_file_size(); i += n)
        {
            if ((n = read(fd, _buffer + i, 1024)) < 0)
                break;
        }
        buffer.insert(buffer.begin(), _buffer, _buffer + response->getbody_file_size());
    }
    close(fd);
    return buffer;
}

void GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    std::cout << B_green << "IM DOING GET REQUEST" << B_def << std::endl;
    if (!request->getPath().empty())
    {
        char *path = (char *)malloc(sizeof(char) * (1000));
        std::cout << B_red << "root path = {" << config->get_server_vect()[index_server].get_root().c_str() << "}" << B_def << std::endl;
        strcpy(path, config->get_server_vect()[index_server].get_root().c_str());
        struct stat st;
        if (request->getPath() == "/")
            strcpy(path + (strlen(path)), "index.html");
        else
            strcpy(path + (strlen(path) - 1), request->getPath().c_str());

        stat(path, &st);
        char s2[50];
        
        if (st.st_size > 0 && open(path, O_RDONLY) > 0)
        {
            strcpy(s2, " 200 OK\r\n");
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(path);
            response->setbody_file_size(getFileSize(path));
            // response->setBody(read_by_vector(path, response));
            response->setpath(path);
            std::cout << B_blue << "getFileSize(path): " << getFileSize(path) << B_def << std::endl;

        }
        else
        {
            response->set_requestFuckedUp(true);

            strcpy(s2, " 404 NOT FOUND\r\n");
            char _path[100] = "./errorsPages/404/404.html";
            response->setResponseStatus(s2);
            response->setResponseHeader();
            response->setContentType(_path);
            response->setbody_file_size(getFileSize(_path));
            // response->setBody(read_by_vector(path, response));
            response->setpath(_path);
            std::cout << B_blue << "getFileSize(path): " << getFileSize(_path) << B_def << std::endl;

        }
        free(path);
    }
}

Response response(int new_socket, Request *request, ParseConfig *config, int index_server)
{
    (void)new_socket;
    Response response;
    std::cout << blue << "********** { Response } ***********************" << def << std::endl;
    if (!request->isGoodrequest())
        ERRORresponse(request, &response);
    else if (!(request->getMethod().compare("GET")))
        GETresponse(request, &response, config, index_server);
    else if (request->getMethod().compare("POST") == 0)
        POSTresponse();
    else if (request->getMethod().compare("DELETE") == 0)
        DELETEresponse();
    else
        ERRORresponse(request, &response);
    std::cout << blue << "********** {End Response } ******************" << def << std::endl ;
    return response;
}
