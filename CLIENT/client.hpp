#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

class user
{
private:
    boost::asio::io_context &io_context_;
    boost::asio::ip::tcp::socket socket_;

    boost::asio::io_context context_message;

    /// void receive_messages();

    /// void start_waiting_for_messages();

    /// void send_PM();

    void what_to_do();

    ///   void receive_server_message();

    void start_login_signup();

public:
    user(boost::asio::io_context &io_context);

    ~user();
};