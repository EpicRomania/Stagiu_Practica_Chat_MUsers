#include "server.hpp"

int main()
{
    // sql_component test("server.sqlite3");

    // test.add_username_and_password("skibidi toilet","fanumTAX");
    // test.add_username_and_password("PAM","Harvestcraft");

    // std::cout<<test.get_user(1)<<'\n';
    // std::cout<<test.get_user_password(1)<<'\n';

    // std::cout<<test.get_user(2)<<'\n';
    // std::cout<<test.get_user_password(2)<<'\n';

    // std::cout<<test.get_user(3)<<'\n';
    // std::cout<<test.get_user_password(3)<<'\n';

    // std::cout<<test.get_id_from_username("skibidi toilet")<<'\n';
    // std::cout<<test.get_id_from_username("PAM")<<'\n';
    // std::cout<<test.get_id_from_username("UNDEFINED")<<'\n';

    // test.add_group("PAM","Gyaat");

    // test.send_message("PAM","Gyaat","Lost in the sauce.");

    try
    {

        unsigned short port = 8443;
        boost::asio::io_context io_context;

        server test_server(io_context, port);

        /// test_server.test_sql_in_server();

        test_server.acceptor_start();

        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}