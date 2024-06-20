#include "SQLiteComponent.hpp"

void sql_component::connect(std::string DB_Path)
{
    try{
        this->database=new SQLite::Database("server.sqlite3", SQLite::OPEN_READWRITE);
        this->is_connected=true;
        std::cout<<"Connected\n";
    }
    catch(std::exception & error)
    {
        std::cerr<<error.what()<<'\n';
    }
    
}

void sql_component::disconnect()
{
    if(this->database)
    {
        delete this->database;
        this->database=nullptr;
    }
    this->is_connected=false;
    std::cout<<"Disconected"<<'\n';
}


void sql_component::reconnect(std::string DB_Path)
{
    disconnect();
    connect(DB_Path);
}


sql_component::sql_component(std::string DB_Path)
{
    connect(DB_Path);

    while(!this->is_connected)
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
   

    try{
        SQLite::Transaction transaction(*this->database);

        SQLite::Statement query_username(*this->database,"INSERT INTO Users (Username) VALUES (?)");
        query_username.bind(1,username);
        query_username.exec();

        int ID=this->database->getLastInsertRowid();

        SQLite::Statement query_password(*this->database,"INSERT INTO Passwords (ID, Password) VALUES (?, ?)");
        query_password.bind(1,ID);
        query_password.bind(2,password);
        query_password.exec();

        transaction.commit();
        std::cout<<"Transaction successfull!\n";
    }
    catch(std::exception & error)
    {
        std::cerr<<error.what()<<'\n';
    }
}

std::string sql_component::get_user(int id)
{
    std::string username{};
    try
    {
        SQLite::Statement query_username(*this->database, "SELECT Username FROM Users WHERE ID = ?");
        query_username.bind(1,id);
        if(query_username.executeStep())
        {
            username = query_username.getColumn(0).getText();
            std::cout<<"got a username from DB : "<<username<<'\n';
        }
        else
        {
           username="NULL";
        }
    }
    catch(std::exception & error)
    {
        std::cerr<<error.what()<<'\n';
    }
    return username;
}

std::string sql_component::get_user_password(int id)
{
    std::string password{};
    try
    {
        SQLite::Statement query_password(*this->database, "SELECT Password FROM Passwords WHERE ID = ?");
        query_password.bind(1,id);
        if(query_password.executeStep())
        {
            password=query_password.getColumn(0).getText();
            std::cout<<"Got a password for user id : "<<id<<'\n';
        }
        else{
            password="NULL";
        }
    }
    catch(std::exception & error)
    {
        std::cerr<<error.what()<<'\n';
    }
    return password;
}
