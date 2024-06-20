#include "server.hpp"

int main()
{
    sql_component test("server.sqlite3");

    //test.add_username_and_password("skibidi toilet","fanumTAX");
    test.add_username_and_password("PAM","Harvestcraft");

    std::cout<<test.get_user(1)<<'\n';
    std::cout<<test.get_user_password(1)<<'\n';

    std::cout<<test.get_user(2)<<'\n';
    std::cout<<test.get_user_password(2)<<'\n';


    std::cout<<test.get_user(3)<<'\n';
    std::cout<<test.get_user_password(3)<<'\n';

    return 0;
}