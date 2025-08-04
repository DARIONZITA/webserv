#include "server.hpp"
#include <cctype>
#include <stringstream>
using namespace std;

vector<string> split_string(string str, string delimiter)
{
    size_t inicio=0, fim;
    vector<string> result;
    do
    {
        fim = str.find(delimiter);
        result.push_back(str.substr(inicio, fim < 0 ? fim : fim - inicio));
        inicio = fim + delimiter.size();
    } while (fim >= 0);
    return result;
}


std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
        ++start;

    size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        --end;

    return str.substr(start, end - start);
}


int string_to_int_positive(char const *str)
{
    int i;
    std::istringstream ss;
    ss.exceptions(std::ios::failbit | std::ios::badbit);
    ss.str(str);
    ss >> i;
    if (i < 0)
        throw std::out_of_range("Negative numbers are not allowed");
    return i;
}