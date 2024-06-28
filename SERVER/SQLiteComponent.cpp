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
        std::cerr << "User not found\n";
        return;
    }
    try
    {
        SQLite::Transaction transaction(*this->database);

        SQLite::Statement query_adding_group(*this->database, "INSERT INTO Groups (ID_Creator, Group_Name, Is_Active) VALUES (?, ?, 1)");
        query_adding_group.bind(1, ID);
        query_adding_group.bind(2, group_name);
        query_adding_group.exec();

        add_User_to_group(group_name, ID);
        transaction.commit();
        std::cout << "Group created and creator added to group successfully!\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
}

int sql_component::get_group_id(std::string group_name)
{
    int Group_ID{};
    try
    {

        SQLite::Statement query_group_name_to_group_id(*this->database, "SELECT Group_ID FROM Groups WHERE Group_Name = ?");
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

void sql_component::add_User_to_group(std::string Group_Name, int User_ID)
{
    int Group_ID = get_group_id(Group_Name);
    if (Group_ID == -1)
    {
        std::cerr << "Group not found\n";
        return;
    }
    try
    {
        SQLite::Statement query(*this->database, "INSERT INTO Groups_Participants (Group_ID, ID_Participant) VALUES (?, ?)");
        query.bind(1, Group_ID);
        query.bind(2, User_ID);
        query.exec();
        std::cout << "User added to group successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

std::vector<std::pair<int, std::string>> sql_component::get_user_groups(int User_ID)
{
    std::vector<std::pair<int, std::string>> groups;
    try
    {
        SQLite::Statement query(*this->database, "SELECT Groups.Group_ID, Groups.Group_Name FROM Groups JOIN Groups_Participants ON Groups.Group_ID = Groups_Participants.Group_ID WHERE Groups_Participants.ID_Participant = ?");
        query.bind(1, User_ID);
        while (query.executeStep())
        {
            int group_id = query.getColumn(0).getInt();
            std::string group_name = query.getColumn(1).getText();
            groups.push_back(std::make_pair(group_id, group_name));
        }
        std::cout << "Groups retrieved successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return groups;
}

std::string sql_component::get_group_name(int group_id)
{
    std::string group_name;
    try
    {
        SQLite::Statement query(*this->database, "SELECT Group_Name FROM Groups WHERE Group_ID = ?");
        query.bind(1, group_id);
        if (query.executeStep())
        {
            group_name = query.getColumn(0).getText();
            std::cout << "Got group name from DB: " << group_name << '\n';
        }
        else
        {
            std::cout << "Failed to get group name from DB\n";
            group_name = "NULL";
        }
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    return group_name;
}

void sql_component::send_DM(int ID_Sender, int ID_Receiver, std::string message)
{
    try
    {
        SQLite::Transaction transaction(*this->database);

        auto timestamp = std::chrono::system_clock::now();
        auto timestamp_c = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&timestamp_c), "%Y-%m-%d %H:%M:%S");
        std::string timestamp_str = ss.str();

        SQLite::Statement query(*this->database, "INSERT INTO Messages_DM (ID_Sender, ID_Receiver, Timestamp, Message) VALUES (?, ?, ?, ?)");
        query.bind(1, ID_Sender);
        query.bind(2, ID_Receiver);
        query.bind(3, timestamp_str);
        query.bind(4, message);

        query.exec();

        transaction.commit();
        std::cout << "Direct message sent successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

std::vector<std::string> sql_component::get_user_messages(int User_ID)
{
    std::vector<std::string> messages;
    try
    {
        SQLite::Statement query(*this->database, "SELECT Users.Username, Messages_DM.Timestamp, Messages_DM.Message FROM Messages_DM JOIN Users ON Messages_DM.ID_Sender = Users.ID WHERE ID_Receiver = ?");

        query.bind(1, User_ID);

        while (query.executeStep())
        {
            std::string entry = query.getColumn(0).getText();
            entry += " ";
            entry += query.getColumn(1).getText();
            entry += " ";
            entry += query.getColumn(2).getText();
            entry += "\n";
            messages.push_back(entry);
        }
        std::cout << "Messages retrieved successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error retrieving messages: " << e.what() << '\n';
    }
    return messages;
}

std::vector<std::string> sql_component::get_MDM(int group_ID)
{
    std::vector<std::string> messages;
    try
    {
        SQLite::Statement query(*this->database, "SELECT ID_Sender, Timestamp, Message "
                                                 "FROM Messages "
                                                 "WHERE Group_ID = ? "
                                                 "ORDER BY Timestamp");

        query.bind(1, group_ID);

        while (query.executeStep())
        {
            int id_sender = query.getColumn(0).getInt();
            std::string timestamp = query.getColumn(1).getText();
            std::string message = query.getColumn(2).getText();

            std::string username = get_user(id_sender);

            std::string formatted_message = username;
            formatted_message += " | ";
            formatted_message += timestamp;
            formatted_message += " | ";
            formatted_message += message;
            formatted_message += "\n";
            messages.push_back(formatted_message);
        }
        std::cout << "Group messages retrieved successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error retrieving group messages: " << e.what() << '\n';
    }
    return messages;
}
