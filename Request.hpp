#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <utility>
#include "includes.hpp"

class Request
{
    private:
        string                              _method;
        string                              _request_URI;
        string                              _HTTP_Version;
        static map<string, vector<string>>  valid_options;
        map<string, string>                 general_headers;
        map<string, string>                 request_headers;
        map<string, string>                 entity_headers;
        string                              entity_body;
        string                              message_body;
        static size_t                       MAX_HEADER_SIZE;
        static size_t                       MAX_BODY_SIZE;
    public:
        Request(string &buffer);
        void get_method();
        static Request *read_request(int client_fd);
        void verify_request_line(string &request_Line);
        int read_headers(vector<string> &lines);
        void specific_checks(void);
        void action(void);
        void get_body(int index_body, vector<string> &lines);
        static void define_size(size_t max_body_size, size_t max_header_size);
        ~Request();
};

#endif