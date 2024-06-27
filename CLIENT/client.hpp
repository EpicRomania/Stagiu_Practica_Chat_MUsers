#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <unistd.h>

class user
{
private:
    boost::asio::io_context &io_context_;
    boost::asio::ip::tcp::socket socket_;

    std::string get_data(boost::asio::ip::tcp::socket &socket);
    void send_data(boost::asio::ip::tcp::socket &socket, const std::string &message);

    void respone_next();

    void receive_users();

    void receive_user_groups();

    void receive_user_groups_mdms();

    void dm_user();

    void show_dms();

    void create_group();

    void add_users_to_group();

    void send_mdm();

    void show_mdms();

    void what_to_do();

    void start_login_signup();

public:
    user(boost::asio::io_context &io_context);

    ~user();
};