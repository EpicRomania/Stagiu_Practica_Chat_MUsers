#include "client.hpp"


int main() {

    
    boost::asio::io_context io_context;
    user client(io_context);

    return 0;
}