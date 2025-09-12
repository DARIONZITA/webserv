#include "HttpFormat.hpp"
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
        m["HttpFormat-header"] = vector<string>(levels, levels + 19);
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

map<string, vector<string> > HttpFormat::valid_options = create_valid_options();


// -1 if after headers dont have body, n::pos other wise
int    HttpFormat::read_headers(vector<string> &lines)
{
    vector<string>::iterator    res;
    vector<string>              params;

    for (size_t i = 1; i < lines.size(); i++)
    {
        params = split_string(lines[i], ":");
        if (!lines[i][0])//empty line before body message
            return (i + 1);
        if( params.size() < 2)
            throw runtime_error("Bad HttpFormat");
        
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
            res = find(valid_options["operacional-header"].begin(), valid_options["HttpFormat-header"].end(), params[0]);
            if (res != valid_options["operacional-header"].end())
                operacional_headers[params[0]] += operacional_headers[params[0]][0] ? ", " + params[1] : params[1];
            else
                entity_headers[params[0]] += entity_headers[params[0]][0] ? ", " + params[1] : params[1];
        }
        else
            general_headers[params[0]] += general_headers[params[0]][0] ? ", " + params[1] : params[1]; 
        
    }
    return -1;
}

void HttpFormat::get_body(int index_body, vector<string> &lines)
{
    for(size_t i = index_body + 1; i < lines.size(); i+=2)
    {
        lines[index_body] += lines[i] + "/r/n";
        this->message_body = lines[index_body];
    }
    
    int size;
    if (general_headers.find("Transfer-Encoding") != general_headers.end())
    {
        for(size_t i = index_body + 1; i < lines.size(); i+=2)
        {
            size = hex_to_int_positive(lines[i].c_str());
            lines[i + 1] += "/r/n";
            entity_body += lines[i + 1].substr(0, size);
        }
    }
    else
    {
        size = string_to_int_positive(entity_headers["Content-Length"].c_str());
        entity_body = this->message_body.substr(0, size);
    }
}


HttpFormat::HttpFormat(){

}

HttpFormat::HttpFormat(string &buffer)
{
    vector<string> lines = split_string(buffer, "\r\n");
    string HttpFormat_Line = lines[0];
    verify_HttpFormat_line(HttpFormat_Line);
    int index_body = read_headers(lines);
    specific_checks();
    get_body(index_body, lines);
}

HttpFormat::~HttpFormat()
{
}

