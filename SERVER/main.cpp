#include "server.hpp"

int main()
{
    sql_component test("server.sqlite3");

    // test.add_username_and_password("skibidi toilet", "fanumTAX");
    // test.add_username_and_password("PAM", "Harvestcraft");

    // std::cout << test.get_user(1) << '\n';
    // std::cout << test.get_user_password(1) << '\n';

    // std::cout << test.get_user(2) << '\n';
    // std::cout << test.get_user_password(2) << '\n';

    // std::cout << test.get_user(3) << '\n';
    // std::cout << test.get_user_password(3) << '\n';

    // std::cout << test.get_id_from_username("skibidi toilet") << '\n';
    // std::cout << test.get_id_from_username("PAM") << '\n';
    // std::cout << test.get_id_from_username("UNDEFINED") << '\n';

    // test.add_group("PAM", "Gyaat");

    // test.send_message("PAM", "Gyaat", "Lost in the sauce.");

    // int skibidi_toilet_id = test.get_id_from_username("skibidi toilet");

    // test.add_User_to_group("Gyaat", skibidi_toilet_id);

    // test.add_User_to_group("Gyaat", 1);
    // test.add_User_to_group("Gyaat", 2);

    // auto groups1 = test.get_user_groups(1);
    // for (auto &group : groups1)
    // {
    //     std::cout << "User 1 Group ID: " << group.first << ", Name: " << group.second << '\n';
    // }

    // auto groups2 = test.get_user_groups(2);
    // for (auto &group : groups2)
    // {
    //     std::cout << "User 2 Group ID: " << group.first << ", Name: " << group.second << '\n';
    // }

    // test.send_DM(1, 2, "sigma sigma!\nAgain");

    // test.send_DM(2, 1, "edge edge!\nAgain");

    // auto messages_user1 = test.get_user_messages(1);
    // std::cout << "Messages for user 1:\n";
    // for (auto &msg : messages_user1)
    // {
    //     std::cout << msg;
    // }

    // auto messages_user2 = test.get_user_messages(2);
    // std::cout << "Messages for user 2:\n";
    // for (auto &msg : messages_user2)
    // {
    //     std::cout << msg;
    // }
    try
    {

        unsigned short port = 8443;
        boost::asio::io_context io_context;

        server test_server(io_context, port);

        test_server.acceptor_start();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}