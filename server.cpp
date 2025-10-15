//#include "server.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>

#include <vector>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;
int main() {

    struct sockaddr_in info_addr;
    struct sockaddr_in addr_clien;
    socklen_t addrlen = sizeof(addr_clien);
    info_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    info_addr.sin_family = AF_INET;
    info_addr.sin_port = htons(8080);
    memset(info_addr.sin_zero, 0, sizeof(info_addr.sin_zero));
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        std::cerr << "Error in creation of the socket" << endl;
        return -1;
    }
    int optval=1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        cerr << "Error in set socket option" << endl;
        return -1;
    }
    if (bind(socket_fd, (sockaddr *)&info_addr, sizeof(info_addr)) < 0)
    {
        cerr << "Error in socket configuration" << endl;
        return -1;
    }
    if (listen(socket_fd, SOMAXCONN) < 0)
    {
        cerr << "Error in socket configuration" << endl;
        return -1;
    }
    string  result;
    string  buffer(1024,0);
    while(true)
    {
        int fd_client = accept(socket_fd, (sockaddr *)&addr_clien, &addrlen);
        while (recv(fd_client, &(buffer[0]), buffer.size(), 0) > 0)
        {
            result += buffer;
            buffer = "";
        }

        

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 72\r\n"
            "\r\n"
            "<html>"
            "<head><title>Olá</title></head>"
            "<body><h1>Servidor funcionando!</h1></body>"
            "</html>";

        send(fd_client, response, strlen(response), 0);
    }

    return 0;
}
