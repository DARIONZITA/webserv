#include "Request.hpp"
#include "Response.hpp"
class Client
{
    private:
        int fd_client
        Request request;
        Response response;
    public:
        Client(/* args */);
        ~Client();
};

Client::Client(/* args */)
{
}

Client::~Client()
{
}
