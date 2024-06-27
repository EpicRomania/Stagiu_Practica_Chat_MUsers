#include "client.hpp"

int main()
{

    try
    {
        boost::asio::io_context io_context;
        user user(io_context);
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}