#include "Epoll.hpp"
using namespace std;
Epoll::Epoll():_n_monitoring(0), _epfd(epoll_create(42)){}

int Epoll::add_fd(int fd, unsigned int events, typeFd type)
{
    struct epoll_event event;
    fcntl(fd, F_SETFL, O_NONBLOCK);
    event.events = events;
    event.data.ptr = new Connection(fd, type); 
    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    _n_monitoring++;

    cout << "log: new " << (type == SERVER? "server": "client")<< " with fd:" << fd << " added for monitoring I/O" << endl;
    return 0;
}

int Epoll::remove_fd(int fd)
{
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
    return 0;
}

int Epoll::wait()
{
    events.resize(_n_monitoring);
    int qtd_fds = epoll_wait(_epfd, events.data(), events.size(), -1);
    cout << "log: "<< qtd_fds << " new event detected!!" << endl;
    return (qtd_fds);
}




Epoll::~Epoll()
{
}
