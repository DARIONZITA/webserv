#include "webserv.hpp"

int main() {
    Server webserv("8080", "0.0.0.0");
    Server webserv2("8000", "0.0.0.0");
    Server::monitoring_fds();
    return 0;
}
