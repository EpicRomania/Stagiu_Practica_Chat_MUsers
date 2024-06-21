#pragma once

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/crypto.h>

#include "group_basic.hpp"  /// grup basic , include log uri , nu se sterge niciodata
#include "group_no_log.hpp" /// la fel ca cel anterior , doar ca nu tine log uri , chat ul e dinaminc , se sterge fie la restartarea servereului
                            /// fie la cererea ownerului sau a adminilor ( adminii pot doar sa goleasca chat ul , nu si sa l stearga )


#include "SQLiteComponent.hpp" /// componenta de SQL a serverrului

class server
{
public:
    std::vector<int> user_ids{};

private:
    server();
    ~server();
};
