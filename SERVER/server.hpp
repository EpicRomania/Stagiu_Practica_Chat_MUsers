#pragma once

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/crypto.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <thread>
#include <mutex>


#include "group_basic.hpp"  /// grup basic , include log uri , nu se sterge niciodata
#include "group_no_log.hpp" /// la fel ca cel anterior , doar ca nu tine log uri , chat ul e dinaminc , se sterge fie la restartarea servereului
                            /// fie la cererea ownerului sau a adminilor ( adminii pot doar sa goleasca chat ul , nu si sa l stearga )
#include "SQLiteComponent.hpp" /// componenta de SQL a serverrului



class server: public std::enable_shared_from_this<server>
{
private:
    
    boost::asio::io_context& io_context_;
    boost::asio::io_context checker_io_context;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> sockets;
    std::mutex sockets_mutex;

    sql_component* server_sql=nullptr;
    
    std::vector<std::pair<int,std::string>> users{}; 
    
    void delete_user( std::string ip); 


    void monitor_user_connections();

    void start_monitoring();

    void add_id_to_list(int id_user,std::string ip);

    void login(std::shared_ptr<boost::asio::ip::tcp::socket> socket, bool was_warning_sent );
    
    void signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket, bool was_warning_sent );

    void login_or_signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket);


public:
    void acceptor_start();
    
    server(boost::asio::io_context & context ,unsigned short port);

    void test_sql_in_server();

    ~server();
};
