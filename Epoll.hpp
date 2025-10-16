#ifndef EPOLL_HPP
#define EPOLL_HPP
#ifndef WEBSERV_HPP
    #include "webserv.hpp"
#endif

typedef enum e_typeFd
{
    CLIENT,
    SERVER
} typeFd;

class Connection
{
    public:
        int _fd;
        e_typeFd _type;
        Connection(int fd, e_typeFd type):_fd(fd),_type(type){};
};

class Epoll
{
    private:
        int _n_monitoring;
        const int _epfd;
    public:
        std::vector<epoll_event> events;
        Epoll();
        int add_fd(int fd, unsigned int events, typeFd type);
        int remove_fd(int fd);
        int wait();
        ~Epoll();
};
#endif