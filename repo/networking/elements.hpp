#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "../webserve.hpp"

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}



char *readFile(const char * fileName);

class Request
{
private:
    std::string _type;
    char buffer[30000];
    long bytes;
    int request_num;
    int requestStatus;
    std::string _method;
    std::string _path;
    std::string _version;
    std::string _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _accept_language;
    std::string _port;
    std::string _body;

public:
    Request()
    {
        request_num = 0;
        _type = "GET";
        bytes = 0;
        requestStatus = 0;
        _method = "GET";
        _path = "./www/index.html";
        _version = "HTTP/1.1";
        _host = "localhost";
        _connection = "keep-alive";
        _accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8";
        _accept_encoding = "gzip, deflate, br";
        _accept_language = "en-US,en;q=0.9";
        _port = "80";
        _body = "";
    };

    Request &operator++()
    {
        ++request_num;
        return *this;
    }

    void setbytes(long bytes) { this->bytes = bytes; }
    long getbytes() { return bytes; }
    void setbuffer(char *buffer) { std::strcpy(this->buffer, buffer); }
    char *getbuffer() { return buffer; }
    std::string getType() const { return _type; };
    int getRequestNum() const { return request_num; };
    int isGoodrequest() { return (requestStatus); };
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    std::string getHost() const { return _host; };
    std::string getPort() const { return _port; };
    std::string getConnection() const { return _connection; };
};
class Response
{
private:
    char hello[10000];
    // int response_num;
    // int responseStatus;
    std::string version;
    std::string status;
    char *body;
    char *response_buffer;
public:
    Response(): version("HTTP/1.1"), status("200 OK")
    {
        // response_num = 0;
        // responseStatus = 0;
        body = NULL;
        response_buffer = (char *)malloc(sizeof(char) * 30000000);
    };

    const char *getHello() const { return hello; };

    void generateResponse(Request *request)
    {
        char res[100000];

        body = readFile(request->getPath().c_str());
        
        strcpy(res, version.c_str());
        strcpy( res + strlen(version.c_str())  ,status.c_str());
        strcpy( res + strlen(res)  ,readFile(request->getPath().c_str()));
        strcpy(response_buffer, res);

        // std::cout << "**************** Response *****************\n";
        // std::cout << res << std::endl;
        // std::cout << "**************** Response *****************\n";
    }
    size_t getSize() const { return strlen(response_buffer); };
    char *getResponse() const { return response_buffer; };
    void setBody(char *_body) { strcpy(body, _body); };
    char *getBody() { return body; };
};

#endif