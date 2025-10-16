#ifndef WEBSERV_HPP
#define WEBSERV_HPP
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
#include <sys/epoll.h>
#include <fcntl.h>


#include <vector>
#include <string>
#include <map>
#include <stdexcept>
//#include <Request.hpp>

#include "Server.hpp"
#ifdef EPOLL_HPP
    #include "Epoll.hpp"
#endif
#define MAX_REQUEST_SIZE 8192
using namespace std;
vector<string>  split_string(string str, string delimiter);
std::string     trim(const std::string& str);
int             string_to_int_positive(char const *str);
int             hex_to_int_positive(char const *str);
#endif