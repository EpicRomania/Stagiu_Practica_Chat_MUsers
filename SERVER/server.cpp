#include "server.hpp"

void server::delete_user(std::string ip)
{
    std::cout << "Removing a user...\n";
    for (auto it = users.begin(); it != users.end(); it++)
    {
        if (it->second == ip)
        {
            it = users.erase(it);
            std::cout << "Removed a user...\n";
            break;
        }
    }
}

void server::monitor_user_connections()
{
    while (true)
    {
        boost::asio::steady_timer timer(checker_io_context, boost::asio::chrono::seconds(5));
        timer.wait();

        std::cout << "Checking Sockets...\n";

        std::lock_guard<std::mutex> lock(sockets_mutex);
        for (auto it = sockets.begin(); it != sockets.end();)
        {
            std::cout << (*it).get()->remote_endpoint() << '\n';

            boost::asio::socket_base::bytes_readable command(true);
            boost::system::error_code ec;
            (*it)->io_control(command, ec);
            std::size_t bytes_readable = command.get();

            if (ec || bytes_readable == 0)
            {

                char buffer[1];
                size_t len = (*it)->read_some(boost::asio::buffer(buffer, 1), ec);

                if (ec == boost::asio::error::would_block || ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset)
                {

                    std::string ip = (*it).get()->remote_endpoint().address().to_string();

                    delete_user(ip);

                    it = sockets.erase(it);
                    std::cout << "Removed a dead socket.\n";
                    continue;
                }
            }
            ++it;
        }
    }
}

void server::start_monitoring()
{
    std::thread([this]()
                {
        this->monitor_user_connections();
        checker_io_context.run(); })
        .detach();
}

void server::add_id_to_list(int id_user, std::string username)
{
    this->users.push_back(std::make_pair(id_user, username));
}

void server::login(std::shared_ptr<boost::asio::ip::tcp::socket> socket, bool was_warning_sent)
{
    auto buffer = std::make_shared<std::array<char, 256>>();
    if (!was_warning_sent)
    {
        socket->write_some(boost::asio::buffer("110 character limit for the username and 110 for the password "));
        was_warning_sent = true;
    }

    buffer->fill(0);

    socket->write_some(boost::asio::buffer("110 character limit for the username and 110 for the password"));

    socket->async_read_some(boost::asio::buffer(*buffer),
                            [this, socket, buffer, was_warning_sent](const boost::system::error_code &error, std::size_t length)
                            {
                                if (!error && length > 0)
                                {
                                    std::string received_buffer(buffer->data(), length);
                                    std::cout << "Received data: " << received_buffer << std::endl;
                                    try
                                    {
                                        auto received_json = nlohmann::json::parse(received_buffer);

                                        std::string username = received_json["username"];
                                        std::string password = received_json["password"];

                                        int id_user = (*server_sql).get_id_from_username(username);
                                        if (id_user != -1)
                                        {
                                            std::string password_in_db = (*server_sql).get_user_password(id_user);
                                            if (password_in_db == password)
                                            {
                                                socket->write_some(boost::asio::buffer("Login was successful"));
                                                std::cout << "Adding a socket\n";
                                                sockets.push_back(socket);

                                                std::cout << "Adding a user..\n";
                                                std::string ip = socket.get()->remote_endpoint().address().to_string();
                                                add_id_to_list(id_user, ip);
                                            }
                                            else
                                            {
                                                socket->write_some(boost::asio::buffer("Incorrect password"));
                                                std::cout << "Deleting bad user\n";
                                                socket->close();
                                                sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                            }
                                        }
                                        else
                                        {
                                            socket->write_some(boost::asio::buffer("Incorrect username"));
                                            std::cout << "Deleting bad user\n";
                                            socket->close();
                                            sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                        }
                                    }
                                    catch (nlohmann::json::parse_error &e)
                                    {
                                        std::cerr << "JSON Parsing Error: " << e.what() << '\n';
                                        std::cout << "Deleting bad user\n";
                                        socket->close();
                                        sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                    }
                                }
                                else
                                {
                                    std::cerr << "Read failed: " << error.message() << '\n';
                                    std::cout << "Deleting bad user\n";
                                    socket->close();
                                    sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                }
                            });
}

void server::signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket, bool was_warning_sent)
{

    auto buffer = std::make_shared<std::array<char, 256>>();
    if (!was_warning_sent)
    {
        socket->write_some(boost::asio::buffer("110 character limit for the username and 110 for the password "));
        was_warning_sent = true;
    }

    buffer->fill(0);

    socket->async_read_some(boost::asio::buffer(*buffer),
                            [this, socket, buffer, was_warning_sent](const boost::system::error_code &error, std::size_t length)
                            {
                                if (!error && length > 0)
                                {

                                    std::string received_data(buffer->data(), length);
                                    std::cout << "Received data: " << received_data << std::endl;

                                    try
                                    {

                                        auto received_json = nlohmann::json::parse(received_data);

                                        std::string password = received_json["password"];
                                        std::string username = received_json["username"];

                                        std::cout << "\nUsername :\t" << username << "\nPassword:\t" << password << '\n';

                                        int id_user = (*server_sql).get_id_from_username(username);
                                        if (id_user == -1)
                                        {
                                            (*server_sql).add_username_and_password(username, password);

                                            socket->write_some(boost::asio::buffer("Signup was a success"));
                                            std::cout << "Adding a socket\n";
                                            sockets.push_back(socket);

                                            std::cout << "Adding a user..\n";
                                            std::string ip = socket.get()->remote_endpoint().address().to_string();
                                            add_id_to_list(id_user, ip);
                                        }
                                        else
                                        {
                                            socket->write_some(boost::asio::buffer("Username exists,start again"));
                                            std::cout << "Deleting bad user\n";
                                            socket->close();
                                            sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                        }
                                    }
                                    catch (nlohmann::json::parse_error &e)
                                    {
                                        socket->write_some(boost::asio::buffer("JSON Parsing Error: " + std::string(e.what()) + "\n"));
                                        signup(socket, was_warning_sent);
                                    }
                                }
                                else
                                {
                                    std::cerr << "Read failed: " << error.message() << '\n';
                                    socket->close();
                                    sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                }
                            });
}

void server::login_or_signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "Waiting for data \n";
    auto buffer = std::make_shared<std::array<char, 256>>();

    buffer->fill(0);

    socket->async_read_some(boost::asio::buffer(*buffer),
                            [this, socket, buffer](const boost::system::error_code &error, std::size_t length)
                            {
                                if (!error && length > 0)
                                {
                                    std::string received_data(buffer->data(), length);
                                    std::cout << "Received data: " << received_data << std::endl;
                                    try
                                    {
                                        auto received_json = nlohmann::json::parse(received_data);
                                        std::string command = received_json["command"];
                                        if (command == "Login")
                                        {
                                            std::cout << "Login\n";
                                            login(socket, false);
                                        }
                                        else if (command == "Signup")
                                        {
                                            std::cout << "Signup\n";
                                            signup(socket, false);
                                        }
                                        else if (command == "Quit")
                                        {
                                            std::cout << "User Quit \n";
                                            std::cout << "Deleting bad user\n";
                                            socket->close();
                                            sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                        }
                                    }
                                    catch (const nlohmann::json::parse_error &e)
                                    {
                                        std::cerr << "JSON Parsing Error: " << e.what() << " in data: " << received_data << '\n';
                                    }
                                }
                                else
                                {
                                    std::cerr << "Read failed: " << error.message() << '\n';
                                    socket->close();
                                    sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
                                }
                            });
}

void server::acceptor_start()
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);

    acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code &error)
                           {
        if (!error) 
        {
            boost::asio::socket_base::keep_alive option(true);
            socket->set_option(option);
            std::cout << "Successfully accepted a new connection." << std::endl;
            login_or_signup(socket);
        } 
        else 
        {
            std::cerr << "Failed to accept a new connection: " << error.message() << std::endl;
        }
        acceptor_start(); });
}

server::server(boost::asio::io_context &io_context, unsigned short port) : io_context_(io_context), acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    this->server_sql = new sql_component("server.sqlite3");
    if (this->server_sql)
    {
        std::cout << "SQL Component Started\n";
    }

    start_monitoring();

    acceptor_start();
}

void server::test_sql_in_server()
{
    std::cout << (*server_sql).get_user(1) << '\n';
    std::cout << (*server_sql).get_user_password(1) << '\n';

    std::cout << (*server_sql).get_user(2) << '\n';
    std::cout << (*server_sql).get_user_password(2) << '\n';

    std::cout << (*server_sql).get_user(3) << '\n';
    std::cout << (*server_sql).get_user_password(3) << '\n';

    std::cout << (*server_sql).get_id_from_username("skibidi toilet") << '\n';
    std::cout << (*server_sql).get_id_from_username("PAM") << '\n';
    std::cout << (*server_sql).get_id_from_username("UNDEFINED") << '\n';
}

server::~server()
{
    if (this->server_sql != nullptr)
    {
        delete this->server_sql;
        this->server_sql = nullptr;
    }
}