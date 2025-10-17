#ifndef SERVER_HPP
#define SERVER_HPP
#include "webserv.hpp"
#include "Epoll.hpp"

using namespace std;

class Server
{
private:
    static Epoll _epoll;
    int optval;
    int server_fd;
    struct addrinfo *res;
    int n_monitoring;
    void adding_client();
public:
    Server(string port, string adress);
    static void monitoring_fds(void);
    ~Server();
};

#endif