#pragma once

#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>

#include <SQLiteCpp/SQLiteCpp.h> /// libraria pentru baza de date

class sql_component
{
private:
    SQLite::Database *database = nullptr;

    bool is_connected = false;

    void connect(std::string DB_Path);

    void disconnect();

    void reconnect(std::string DB_Path);

public:
    sql_component(std::string DB_Path);
    ~sql_component();

    void add_username_and_password(std::string username, std::string password);

    std::string get_user(int id);

    std::string get_user_password(int id);

    int get_id_from_username(std::string username);

    void add_group(std::string username, std::string group_name);

    void delete_group(); /// doar modifica Is_Active din tabela

    int get_group_id(std::string group_name);

    void send_message(std::string username, std::string group_name, std::string message);
};