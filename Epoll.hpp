#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "includes.hpp"
#include "Server.hpp"
#include "Client.hpp"

typedef enum e_typeFd
{
    CLIENT,
    SERVER
} typeFd;

typedef union u_entity
{
    public:
        Client client;
        Server server;
        entity() {}
        ~entity() {}
} t_entity;

class Connection
{
    private:
    public:
        t_entity entity;
        typeFd _type;
        
        Connection(int fd, typeFd type, ):_fd(fd),_type(type)
        {
            entity.client
        };
};

class Epoll
{
    private:
        int _n_monitoring;
        const int _epfd;
    public:
        std::vector<epoll_event> events;
        Epoll();
        int add_fd(int fd, unsigned int events, typeFd type, int my_client);
        int remove_fd(int fd);
        int wait();
        ~Epoll();
};
#endif