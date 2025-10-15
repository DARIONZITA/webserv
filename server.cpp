//#include "server.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <fcntl.h>



#include <vector>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

enum e_typeFd
{
    CLIENT,
    SERVER
};

class Connection
{
    public:
        int _fd;
        e_typeFd _type;
        Connection(int fd, e_typeFd type):_fd(fd),_type(type){};
};

int main() {
    cerr << "dario" << endl;
    struct sockaddr_in info_addr;
    struct sockaddr_in addr_clien;
    socklen_t addrlen = sizeof(addr_clien);
    info_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    info_addr.sin_family = AF_INET;
    info_addr.sin_port = htons(8080);
    memset(info_addr.sin_zero, 0, sizeof(info_addr.sin_zero));
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        std::cerr << "Error in creation of the socket" << endl;
        return -1;
    }
    int optval=1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        cerr << "Error in set socket option" << endl;
        return -1;
    }
    if (bind(server_fd, (sockaddr *)&info_addr, sizeof(info_addr)) < 0)
    {
        cerr << "Error in socket configuration" << endl;
        return -1;
    }
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        cerr << "Error in socket configuration" << endl;
        return -1;
    }
    string  result;
    int n_monitoring = 0;
    char  buffer[1024];
    int epfd = epoll_create(42);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    struct epoll_event event;

    event.events = EPOLLIN;
    event.data.ptr = new Connection(server_fd, SERVER); 
    epoll_ctl(epfd, EPOLL_CTL_ADD,server_fd , &event);n_monitoring++;
    vector<epoll_event> _events;
    
    while(true)
    {
        cout << "servidor rodando" << endl;
        _events.resize(n_monitoring);
        int qtd_fds = epoll_wait(epfd, _events.data(), _events.size(), -1);
        Connection *element;
        struct epoll_event event;
        for(int i = 0, fd_client; i < qtd_fds; i++)
        {
            element = (Connection *)_events[i].data.ptr;
            if (element->_type == SERVER)
            {
                fd_client = accept(server_fd, (sockaddr *)&addr_clien, &addrlen);
                fcntl(fd_client, F_SETFL, O_NONBLOCK);
                event.events = EPOLLET | EPOLLIN | EPOLLOUT;
                event.data.ptr = new Connection(fd_client, CLIENT);
                epoll_ctl(epfd, EPOLL_CTL_ADD, fd_client, &event);
                
            }
            else if (element->_type == CLIENT)
            {
                if (_events[i].events == (EPOLLET | EPOLLIN))  
                {
                    int siz = recv(element->_fd, &buffer, sizeof(buffer), 0); 
                    while(siz > 0)
                    {
                        result += buffer;
                        siz = recv(element->_fd, &buffer, sizeof(buffer), 0);
                    }
                    if (siz == 0)
                    {
                        //verificar se é keep-alive antes
                        event.events = EPOLLET | EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_MOD, fd_client, &event);
                    }
                }
                else if (_events[i].events == (EPOLLET | EPOLLOUT))
                {
                    string response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 72\r\n"
                        "\r\n"
                        "<html>"
                        "<head><title>Olá</title></head>"
                        "<body><h1>Servidor funcionando!</h1></body>"
                        "</html>";

                    int siz = send(fd_client, response.c_str(), response.size(), 0);
                    if (siz == 0)
                    {
                        //verificar se é keep-alive antes
                        epoll_ctl(epfd, EPOLL_CTL_ADD, fd_client, &event);
                        close(fd_client);
                    }
                
                }
            }
        }
    }

    return 0;
}
