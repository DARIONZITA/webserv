#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#define MAX_REQUEST_SIZE 8192
using namespace std;
vector<string>  split_string(string str, string delimiter);
std::string     trim(const std::string& str);
int             string_to_int_positive(char const *str);
#endif