#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <string>
#include <utility>
#include "server.hpp"


class Response
{
    private:
        string                              _status_code;
        string                              _HTTP_Version;
        static map<string, string>          Reason_Phrases;
        static map<string, vector<string>>  valid_options;
        map<string, string>                 general_headers;
        map<string, string>                 response_headers;
        map<string, string>                 entity_headers;
        string                              entity_body;
        string                              message_body;
    public:
        Response(char *buffer);
        static Request  *read_request(int client_fd);
        void            verify_request_line(string &request_Line);
        int             read_headers(vector<string> &lines);
        void            specific_checks(void);
        void            action(void);
        void            get_body(int index_body, vector<string> &lines);
        
        ~Request();
};

#endif