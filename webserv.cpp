#include "webserv.hpp"

int main() {
    //ler o arquivo de cofiguração
    // criar os N servidores com as suas especificidades
    Server webserv("8080", "0.0.0.0");
    Server webserv2("8000", "0.0.0.0");
    Server::monitoring_fds();
    return 0;
}
