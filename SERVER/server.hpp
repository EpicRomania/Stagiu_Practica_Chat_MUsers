#pragma once

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/crypto.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <unistd.h>

#include "group_basic.hpp"     /// grup basic , include log uri , nu se sterge niciodata
#include "group_no_log.hpp"    /// la fel ca cel anterior , doar ca nu tine log uri , chat ul e dinaminc , se sterge fie la restartarea servereului
                               /// fie la cererea ownerului sau a adminilor ( adminii pot doar sa goleasca chat ul , nu si sa l stearga )
#include "SQLiteComponent.hpp" /// componenta de SQL a serverrului

class server : public std::enable_shared_from_this<server>
{
private:
    boost::asio::io_context &io_context_;
    boost::asio::io_context checker_io_context;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> sockets;
    std::mutex sockets_mutex;

    sql_component *server_sql = nullptr;

    std::vector<std::pair<int, std::string>> users{};
    std::mutex mutex_users;

    void check_socket(boost::asio::ip::tcp::socket &socket); /// semi inutila daca clientul inchide conexiunea abrupt , in caz de ceva se poate sterge

    void send_active_users(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void send_user_groups(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void send_user_group_mdms(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    std::string get_data(boost::asio::ip::tcp::socket &socket);

    void send_data(boost::asio::ip::tcp::socket &socket, const std::string &message);

    void dm_user(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void show_dms(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void create_group(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void add_users_to_group(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void send_mdm(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void show_mdms(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void next_up(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void delete_user(std::string ip);

    void monitor_user_connections();

    void start_monitoring();

    void add_id_to_list(int id_user, std::string ip);

    void login(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void login_or_signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

public:
    void acceptor_start();

    server(boost::asio::io_context &context, unsigned short port);

    /// void test_sql_in_server();

    ~server();
};