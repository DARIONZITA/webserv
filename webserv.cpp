#include "webserv.hpp"

int main() {
    Server webserv("8080", "0.0.0.0");

    webserv.monitoring_fds();

    return 0;
}
