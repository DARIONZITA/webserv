#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include "includes.hpp"
//#include <Request.hpp>

#include "Epoll.hpp"
#include "Server.hpp"
#define MAX_REQUEST_SIZE 8192

using namespace std;
vector<string>  split_string(string str, string delimiter);
std::string     trim(const std::string& str);
int             string_to_int_positive(char const *str);
int             hex_to_int_positive(char const *str);
#endif