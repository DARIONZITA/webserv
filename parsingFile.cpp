#include <fstream>
#include <vector>
#include <string>
#include "server.hpp"



class parsingFile
{
private:
    /* data */
public:
    parsingFile(/* args */);
    ~parsingFile();
};

parsingFile::parsingFile(/* args */)
{
}

parsingFile::~parsingFile()
{
}

tag geral 
tags server
tags 
void  parsingFileConfig(std::ifstream &file_config)
{

}

int main(int argc, char **argv)
{
    if(argc > 2)
        return 1;
    //if(argc == 1)
        //arquivo de configuraćão padrão
    std::ifstream file_config(argv[1]);
    
    if (file_config.is_open())
    {
        return -1;
    }
    parsingFileConfig(file_config);

}



tag:
    list_tags
    atributos