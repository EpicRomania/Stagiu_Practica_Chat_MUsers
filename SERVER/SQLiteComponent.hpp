#pragma once

#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <vector>
#include <sstream>
#include <iomanip>

#include <SQLiteCpp/SQLiteCpp.h> /// library for the database

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

    int get_group_id(std::string group_name);

    void send_message(std::string username, std::string group_name, std::string message);

    void add_User_to_group(std::string Group_Name, int User_ID);

    std::vector<std::pair<int, std::string>> get_user_groups(int User_ID);

    void send_DM(int ID_Sender, int ID_Receiver, std::string message);

    std::string get_group_name(int group_id);

    std::vector<std::string> get_user_messages(int User_ID);
};
