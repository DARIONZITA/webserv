#include "server.hpp"
#include <fstream>

class ParsingFile
{
private:
    /* data */
    map<string, string> global_vars;
    vector<map<string, string>> server_vars;
    vector<vector<map<string, string>>> locations;
public:
    ParsingFile(ifstream &file);
    ~ParsingFile();
};

ParsingFile::ParsingFile(ifstream &file)
{
    string line;
    map<string, string> *my_moment;
    int index=0;

    while (getline(file,line))
    {
        line = trim(line);
        if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            if (line == "[server]"){
                server_vars.push_back(map<string, string>());
                my_moment = &(server_vars.back());
                locations.push_back(vector<map<string, string>>());
            }
            else if (line == "[global]")
                my_moment = &global_vars;
            else if (line == "[location]")
            {
                locations.back().push_back(map<string, string>());
                my_moment = &(locations.back().back());
            }
            else
                throw runtime_error("Bad format of the configuration file");
        }
        else{
            vector<string> parts = split_string(line, "=");
            if (parts.size() < 2)
                throw runtime_error("Bad format of the configuration file");
            if (parts.size() > 2)
            {
                for(int i=2; i < parts.size(); i++)
                    parts[1] += "=" + parts[2];
            }
            (*my_moment)[trim(parts[0])] = trim(parts[1]);
        }
        
    }
    
}

ParsingFile::~ParsingFile()
{
}