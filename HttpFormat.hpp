#ifndef HTTPFORMAT_HPP
#define HTTPFORMAT_HPP
#include <string>
#include <utility>
#include "server.hpp"


class HttpFormat
{
    protected:
        string                              _HTTP_Version;
        static map<string, vector<string>>  valid_options;
        map<string, string>                 general_headers;
        map<string, string>                 operacional_headers;
        map<string, string>                 entity_headers;
        string                              entity_body;
        string                              message_body;
    public:
        HttpFormat();
        HttpFormat(string &buffer);
        int read_headers(vector<string> &lines);
        virtual void specific_checks(void) = 0;
        void get_body(int index_body, vector<string> &lines);
        virtual ~HttpFormat();
};

#endif