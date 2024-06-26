#include "SQLiteComponent.hpp"

void sql_component::connect(std::string DB_Path)
{
    try
    {
        this->database = new SQLite::Database("server.sqlite3", SQLite::OPEN_READWRITE);
        this->is_connected = true;
        std::cout << "Connected\n";
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
}

void sql_component::disconnect()
{
    if (this->database)
    {
        delete this->database;
        this->database = nullptr;
    }
    this->is_connected = false;
    std::cout << "Disconected" << '\n';
}

void sql_component::reconnect(std::string DB_Path)
{
    disconnect();
    connect(DB_Path);
}

sql_component::sql_component(std::string DB_Path)
{
    connect(DB_Path);

    while (!this->is_connected)
    {
        reconnect(DB_Path);
    }
}

sql_component::~sql_component()
{
    disconnect();
}

void sql_component::add_username_and_password(std::string username, std::string password)
{

    try
    {
        SQLite::Transaction transaction(*this->database);

        SQLite::Statement query_username(*this->database, "INSERT INTO Users (Username) VALUES (?)");
        query_username.bind(1, username);
        query_username.exec();

        int ID = this->database->getLastInsertRowid();

        SQLite::Statement query_password(*this->database, "INSERT INTO Passwords (ID, Password) VALUES (?, ?)");
        query_password.bind(1, ID);
        query_password.bind(2, password);
        query_password.exec();

        transaction.commit();
        std::cout << "Transaction successfull!\n";
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
}

std::string sql_component::get_user(int id)
{
    std::string username{};
    try
    {
        SQLite::Statement query_username(*this->database, "SELECT Username FROM Users WHERE ID = ?");
        query_username.bind(1, id);
        if (query_username.executeStep())
        {
            username = query_username.getColumn(0).getText();
            std::cout << "got a username from DB : " << username << '\n';
        }
        else
        {
            std::cout << "Failed to get Username from DB\n";
            username = "NULL";
        }
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    return username;
}

std::string sql_component::get_user_password(int id)
{
    std::string password{};
    try
    {
        SQLite::Statement query_password(*this->database, "SELECT Password FROM Passwords WHERE ID = ?");
        query_password.bind(1, id);
        if (query_password.executeStep())
        {
            password = query_password.getColumn(0).getText();
            std::cout << "Got a password for user id : " << id << '\n';
        }
        else
        {
            std::cout << "Failed to get Password from DB\n";
            password = "NULL";
        }
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    return password;
}

int sql_component::get_id_from_username(std::string username)
{
    int ID{};
    try
    {
        SQLite::Statement query_username_to_id(*this->database, "SELECT ID FROM Users WHERE Username = ?");
        query_username_to_id.bind(1, username);
        if (query_username_to_id.executeStep())
        {
            ID = query_username_to_id.getColumn(0).getInt();
            std::cout << "Got an ID from the DB : " << ID << '\n';
        }
        else
        {
            std::cout << "Failed to get ID from DB\n";
            ID = -1;
        }
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    return ID;
}

void sql_component::add_group(std::string username, std::string group_name)
{
    int ID = get_id_from_username(username);
    if (ID == -1)
    {
        std::cerr << "User not FOund\n";
        return;
    }
    try
    {

        SQLite::Transaction transaction(*this->database);

        SQLite::Statement query_adding_group(*this->database, "INSERT INTO Groups (ID_Creator, Group_Name, Is_Active) VALUES (?, ?, 1)");

        query_adding_group.bind(1, ID);
        query_adding_group.bind(2, group_name);

        query_adding_group.exec();

        transaction.commit();
        std::cout << "Transaction Successfull!\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
}

void sql_component::delete_group()
{
}

int sql_component::get_group_id(std::string group_name)
{
    int Group_ID{};
    try
    {
        SQLite::Statement query_group_name_to_group_id(*this->database, "Select Group_ID FROM Groups WHERE Group_Name = ?");
        query_group_name_to_group_id.bind(1, group_name);
        if (query_group_name_to_group_id.executeStep())
        {
            Group_ID = query_group_name_to_group_id.getColumn(0).getInt();
            std::cout << "Got a Group_ID from DB : " << Group_ID << '\n';
        }
        else
        {
            std::cout << "Failed to get Group_ID from DB\n";
            Group_ID = -1;
        }
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    return Group_ID;
}

void sql_component::send_message(std::string username, std::string group_name, std::string message)
{
    int ID = get_id_from_username(username);
    if (ID == -1)
    {
        std::cerr << "USer not Found\n";
        return;
    }
    int Group_ID = get_group_id(group_name);
    if (Group_ID == -1)
    {
        std::cerr << "Group Not Found\n";
        return;
    }

    try
    {
        SQLite::Transaction transaction(*this->database);

        auto timestamp = std::chrono::system_clock::now();
        auto timestamp_c = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&timestamp_c), "%Y-%m-%d %H:%M:%S");
        std::string timestamp_str = ss.str();

        SQLite::Statement query_adding_message(*this->database, "INSERT INTO Messages (Group_ID, ID_Sender, Timestamp, Message) VALUES (?, ?, ?, ?)");

        query_adding_message.bind(1, Group_ID);
        query_adding_message.bind(2, ID);
        query_adding_message.bind(3, timestamp_str);
        query_adding_message.bind(4, message);

        query_adding_message.exec();

        transaction.commit();
        std::cout << "Transaction SUccessfull\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
