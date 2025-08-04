#include "Request.hpp"
#include <algorithm>
static map<string, vector<string> > create_valid_options() {
    map<string, vector<string> > m;
    {
        string levels[] = {"GET", "POST", "DELETE"};
        m["method"] = vector<string>(levels, levels + 3);
    }
    {
        string levels[] = {
            "Cache-Control",
            "Connection ",
            "Date",
            "Pragma",
            "Trailer",
            "Transfer-Encoding",
            "Upgrade",
            " Via",
            "Warning"
        };
        m["general-header"] = vector<string>(levels, levels + 9);
    }
    {
        string levels[] = {
            "Accept",
            "Accept-Charset",
            "Accept-Encoding",
            "Accept-Language",
            "Authorization",
            "Expect",
            "From",
            "Host",
            "If-Match",
            "If-Modified-Since",
            "If-None-Match",
            "If-Range",
            "If-Unmodified-Since",
            "Max-Forwards",
            "Proxy-Authorization",
            "Range",
            "Referer",
            "TE",
            "User-Agent"
        };
        m["request-header"] = vector<string>(levels, levels + 19);
    }
    {
        string levels[] = {
            "Allow",
            "Content-Encoding",
            "Content-Language",
            "Content-Length",
            "Content-Location",
            "Content-MD5",
            "Content-Range",
            "Content-Type",
            "Expires",
            "Last-Modified"
        };
        m["entity-header"] = vector<string>(levels, levels + 10);
    }

    return m;
}

map<string, vector<string> > Request::valid_options = create_valid_options();

void    Request::verify_request_line(string& Request_Line)
{
    vector<string> params = split_string(Request_Line, " ");
    if (params.size() != 3)
        throw runtime_error("Bad request: Invalid syntax in request line");
    vector<string>::iterator res = find(valid_options["method"].begin(), valid_options["method"].end(), params[0]);
    if (res == valid_options["method"].end())
        throw runtime_error("Bad request: Can't support method");
    if (params[1][0] != '/')
        throw std::runtime_error("Bad request: invalid abs_path");
    if (params[2] != "HTTP/1.1")
        throw std::runtime_error("Bad request: invalid Http-version");
    _method = params[0];
    _request_URI = params[1];
    _HTTP_Version = parms[2];
}
// -1 if after headers dont have body, n::pos other wise
int    Request::read_headers(vector<string> &lines)
{
    vector<string>::iterator    res;
    vector<string>              params;

    for (size_t i = 1; i < lines.size(); i++)
    {
        params = split_string(lines[i], ":");
        if (!lines[i][0])
            return (i + 1);
        if( params.size() < 2)
            throw runtime_error("Bad request");
        
        if (params.size() > 2)//if the split does more than it should
        {
            for (size_t i = 2; i < params.size(); i++)
                params[1] += ":" + params[i];
        }
        params[1] = trim(params[1]);
        res = find(valid_options["general-header"].begin(), valid_options["general-header"].end(), params[0]);
        // if don't is the first time, add "," then de property
        if (res == valid_options["general-header"].end())
        {
            res = find(valid_options["request-header"].begin(), valid_options["request-header"].end(), params[0]);
            if (res != valid_options["request-header"].end())
                request_headers[params[0]] = request_headers[params[0]][0] ? ", " + params[1] : params[1];
            else
                entity_headers[params[0]] = entity_headers[params[0]][0] ? ", " + params[1] : params[1];
        }
        else
            general_headers[params[0]] = general_headers[params[0]][0] ? ", " + params[1] : params[1]; 
        
    }
    return -1;
}

void Request::specific_checks(void)
{
    if (general_headers.find("Transfer-Encoding") != general_headers.end())
    {
        if (general_headers["Transfer-Enconding"] !=  "chunked")
            throw runtime_error("Bad request: Transfer-Enconding");
    }
}

void Request::get_body(int index_body, vector<string> &lines)
{
    for(size_t i = index_body + 1; i < lines.size(); i+=2)
    {
        lines[index_body] += lines[i];
    }

    if (general_headers.find("Transfer-Encoding") != general_headers.end())
    {
        int size;
      
        size = string_to_int_positive(lines[i]);
        entity_body += lines[i + 1].substr(0, size);

    }
    
    
}

Request::Request(string &buffer)
{
    vector<string> lines = split_string(buffer, "\r\n");
    string Request_Line = lines[0];
    int index_body = verify_request_line(Request_Line);
    read_headers(lines);
    specific_checks();
    get_body(index_body);
}

Request::~Request()
{
}

/*
typedef struct {
    ngx_list_t                        headers;

    ngx_table_elt_t                  *host;
    ngx_table_elt_t                  *connection;
    ngx_table_elt_t                  *if_modified_since;
    ngx_table_elt_t                  *if_unmodified_since;
    ngx_table_elt_t                  *if_match;
    ngx_table_elt_t                  *if_none_match;
    ngx_table_elt_t                  *user_agent;
    ngx_table_elt_t                  *referer;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_range;
    ngx_table_elt_t                  *content_type;

    ngx_table_elt_t                  *range;
    ngx_table_elt_t                  *if_range;

    ngx_table_elt_t                  *transfer_encoding;
    ngx_table_elt_t                  *te;
    ngx_table_elt_t                  *expect;
    ngx_table_elt_t                  *upgrade;

*/