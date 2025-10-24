#include "Server.hpp"
Epoll Server::_epoll;

Server::Server(string port, string adress): optval(1)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_flags = AI_PASSIVE; 
    int status;
    if ((status = getaddrinfo(adress.c_str(), port.c_str(), &hints, &res)))
        throw runtime_error(string("Bad configuration in server ") + gai_strerror(status));
    struct addrinfo *aux = res;
    while(aux)
    {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) ==-1)
        {
            aux = aux->ai_next;
            continue;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        {
            cerr << "Error in set socket option" << endl;
            aux = aux->ai_next;
            continue;
        }
        if (bind(server_fd, aux->ai_addr, aux->ai_addrlen) < 0)
        {
            aux = aux->ai_next;
            cerr << "Error in socket configuration" << endl;
            continue;
        }
        break;
    }
    if (!aux)
        throw runtime_error("Bad configuration in server again");
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        cerr << "Error in socket configuration" << endl;
        throw runtime_error("Error in socket configuration");
    }
    _epoll.add_fd(server_fd, EPOLLIN, SERVER, -1);
}

void Server::monitoring_fds(void)
{
    char  buffer[1024];
    string  result;
    struct sockaddr_in addr_clien;
    socklen_t addrlen = sizeof(addr_clien);
    Connection *element;
    while (true)
    {
        int qtd_fds = _epoll.wait();
        
        for(int i = 0; i < qtd_fds; i++)
        {
            element = (Connection *)_epoll.events[i].data.ptr;
            if (element->_type == SERVER)
            {
                cout << "log: The server have a new connction for accept" << endl;
                int fd_client = accept(element->_fd, (sockaddr *)&addr_clien, &addrlen);
                if (fd_client > 0)
                {
                    _epoll.add_fd(fd_client, EPOLLET | EPOLLIN | EPOLLOUT, CLIENT, element->_fd);
                }
                
            }
            else if (element->_type == CLIENT)
            {
                if ((_epoll.events[i].events)  & (EPOLLET | EPOLLIN))  
                {
                    cout << "log: the client " << element->_fd << " have bytes for read" << endl;
                    int siz = recv(element->_fd, &buffer, sizeof(buffer), 0); 
                    while(siz > 0)
                    {
                        result.append(buffer, siz);
                        siz = recv(element->_fd, &buffer, sizeof(buffer), 0);
                    }
                    cout << "log: The readed was: "<< endl << result << endl;
                    //if (siz == 0)
                    //{
                        //verificar se é keep-alive antes
                        //event.events = EPOLLET | EPOLLIN | EPOLLOUT;
                        //epoll_ctl(epfd, EPOLL_CTL_MOD, fd_client, &event);
                   // }
                   processing_request(result);
                }
                //else if
                if ((_epoll.events[i].events) & (EPOLLOUT | EPOLLET))
                {
                    cout << "log: the client " << element->_fd << " can recive  bytes" << endl;

                    string response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 72\r\n"
                        "\r\n"
                        "<html>"
                        "<head><title>Olá</title></head>"
                        "<body><h1>Servidor funcionando!</h1></body>"
                        "</html>";
                    

                    int siz = send(element->_fd, response.c_str(), response.size(), 0);
                    if (siz == 0 || siz == static_cast<int>(response.size()))
                    {
                        //verificar se é keep-alive antes 
                        _epoll.remove_fd(element->_fd);
                        close(element->_fd);
                       
                        cout << "log: the client " << element->_fd << " removed" << endl;
                    }
                   // else if (siz > 0)
                   // {
                       
                   // }
                
                }
            }
        }

    }
    

}

Server::~Server()
{
    freeaddrinfo(res);

}

