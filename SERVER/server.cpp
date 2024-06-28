#include "server.hpp"

void server::check_socket(boost::asio::ip::tcp::socket &socket)
{
    if (!socket.is_open())
    {
        std::cerr << "oppa o fugit socketu\n";
        return;
    }
}

void server::send_data(boost::asio::ip::tcp::socket &socket, const std::string &message)
{
    check_socket(socket);

    try
    {
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to send data: " << e.what() << std::endl;
    }
}

std::string server::get_data(boost::asio::ip::tcp::socket &socket)
{
    check_socket(socket);

    boost::asio::streambuf buf;
    try
    {
        boost::asio::read_until(socket, buf, "\n");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to read data: " << e.what() << std::endl;
        return "";
    }
    return boost::asio::buffer_cast<const char *>(buf.data());
}

void server::send_active_users(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::lock_guard<std::mutex> lock(mutex_users);
    std::string buffer;

    try
    {
        for (const auto &user : users)
        {
            if (!buffer.empty())
            {
                buffer += " ] ";
            }
            buffer += std::to_string(user.first) + " | " + server_sql->get_user(user.first);
        }
        buffer += "\n";

        send_data(*socket, buffer);

        std::cout << "User data sent. Waiting for ack.\n";

        std::string ack = get_data(*socket);

        if (ack.find("ACK") != std::string::npos)
        {
            std::cerr << "ACK lost :( \n";
            send_data(*socket, "Try again , smth went bad\n");
        }
        else
        {
            std::cout << "\nACK received , moving on \n";
            send_data(*socket, "Go on\n");
            std::string auxil = get_data(*socket);
            std::cout << "remainder buffer clear" << auxil << '\n';
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error sending active users: " << e.what() << '\n';
    }
}

void server::send_user_groups(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "getting a user all the groups he created \n";
    std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

    int user_id = -1;
    for (const auto &user : users)
    {
        if (user.second == ip)
        {
            user_id = user.first;
            std::cout << "User ID found: " << user_id << '\n';
            break;
        }
    }
    std::cout << "Got the ID\n";
    std::vector<std::pair<int, std::string>> buffer_groups = server_sql->get_user_groups(user_id);

    std::string sending_str;

    for (const auto &group : buffer_groups)
    {
        sending_str += std::to_string(group.first);
        sending_str += " | ";
        sending_str += group.second;
        sending_str += " ] ";
    }
    sending_str += "\n";
    send_data(*socket, sending_str);
}

void server::dm_user(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{

    std::cout << "sent usrs\n";
    std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

    int user_id_sender = -1;
    for (const auto &user : users)
    {
        if (user.second == ip)
        {
            user_id_sender = user.first;
            std::cout << "User ID found: " << user_id_sender << '\n';
            break;
        }
    }
    std::cout << "Got the ID\n";

    std::cout << "Waiting for message with the ID\n";
    std::string user_id_receiver = get_data(*socket);
    std::cout << "Received buffer id:\t" << user_id_receiver;

    int user_id_receiver_int = std::stoi(user_id_receiver);
    std::cout << "Receiver:\t" << user_id_receiver_int << '\n';

    std::cout << "Waiting for message\n";
    std::string message = get_data(*socket);

    server_sql->send_DM(user_id_sender, user_id_receiver_int, message);
}

void server::show_dms(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "Getting ID from IP\n";
    std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

    int user_id = -1;
    for (const auto &user : users)
    {
        if (user.second == ip)
        {
            user_id = user.first;
            std::cout << "User ID found: " << user_id << '\n';
            break;
        }
    }
    std::cout << "Got the ID\n";

    std::vector<std::string> buffer_mesaje = server_sql->get_user_messages(user_id);

    std::cout << "Got the vector\n";

    std::string sending_str;

    for (const auto &message : buffer_mesaje)
    {
        sending_str += message;
    }
    sending_str += "\n";
    send_data(*socket, sending_str);
}

void server::create_group(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "Getting ID from IP\n";
    std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

    int user_id = -1;
    for (const auto &user : users)
    {
        if (user.second == ip)
        {
            user_id = user.first;
            std::cout << "User ID found: " << user_id << '\n';
            break;
        }
    }
    std::cout << "Got the ID\n";

    std::string username = server_sql->get_user(user_id);
    std::cout << "USERNAME:\t" << username << '\n';

    std::cout << "Waiting for user to send the groupos name \n";

    std::string group_name = get_data(*socket);

    std::cout << "Got the group's name :\t" << group_name << '\n';

    server_sql->add_group(username, group_name);

    std::cout << "porlly group was created\n";
}

void server::add_users_to_group(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::string data = get_data(*socket);
    std::cout << data << '\n';

    try
    {
        auto received_json = nlohmann::json::parse(data);
        if (received_json.contains("command") && received_json["command"] == "Quit")
        {
            std::cout << "adio add users\n";
            return;
        }

        std::string user_id_str = received_json["User_ID"];
        std::string group_id_str = received_json["Group_ID"];

        int user_id = std::stoi(user_id_str);
        int group_id = std::stoi(group_id_str);

        std::cout << "User ID: " << user_id << ", Group ID: " << group_id << '\n';

        std::string group_name = server_sql->get_group_name(group_id);

        if (user_id != -1 && group_id != -1)
        {
            server_sql->add_User_to_group(group_name, user_id);
        }
        else
        {
            std::cout << "DENIEDr.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to parse JSON data: " << e.what() << '\n';
    }
}

void server::send_mdm(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "going in send_mdm\n";
    std::string data = get_data(*socket);

    if (data.find("QUIT") != std::string::npos)
    {
        std::cout << "User Quit sneding MDMD\n";
        return;
    }

    std::cout << "Getting ID from IP\n";
    std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

    int user_id = -1;
    for (const auto &user : users)
    {
        if (user.second == ip)
        {
            user_id = user.first;
            std::cout << "User ID found: " << user_id << '\n';
            break;
        }
    }
    std::cout << "Got the ID\n";

    std::cout << '\n'
              << data << '\n';

    int group_id = std::stoi(data);

    std::string group_name = server_sql->get_group_name(group_id);

    std::string username = server_sql->get_user(user_id);

    std::string message_rec = get_data(*socket);

    std::cout << "Adding message to DB\n";
    server_sql->send_message(username, group_name, message_rec);
    std::cout << "hopefly it workd\n";
}

void server::show_mdms(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "going in send_mdms\n";

    std::string data = get_data(*socket);

    if (data.find("QUIT") != std::string::npos)
    {
        std::cout << "User Quit sneding MDMD\n";
        return;
    }

    int group_id = std::stoi(data);

    std::vector<std::string> mdms_buffer = server_sql->get_MDM(group_id);

    std::string sending_data;

    for (const auto &buffer : mdms_buffer)
    {
        sending_data += buffer;
    }
    sending_data += "\n";

    send_data(*socket, sending_data);
}

void server::next_up(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    check_socket(*socket);

    std::cout << "IN next\n";
    bool did_user_quit = false;
    while (!did_user_quit)
    {
        std::cout << "In while in next_up\n";
        check_socket(*socket);

        std::string choice = get_data(*socket);

        std::cout << " ce am primti \t" << choice << '\n';
        try
        {
            int ch = std::stoi(choice);

            std::cout << "Choice : " << ch << '\n';

            switch (ch)
            {
            case 1:
            {
                send_active_users(socket);
                std::cout << "Sent user the active ppl\n";

                std::cout << "USer Dm\n";
                dm_user(socket);
                break;
            }
            case 2:
                std::cout << "User create\n";
                show_dms(socket);
                break;
            case 3:
                std::cout << "group create\n";
                create_group(socket);
                break;
            case 4:
            {
                send_active_users(socket);

                std::cout << "prolly  snet the users\n";

                send_user_groups(socket);

                std::cout << "prolly sent the groups idk\n";

                std::cout << "add users to group\n";
                add_users_to_group(socket);
                break;
            }
            case 5:
            {
                std::cout << "sending groups\n";

                send_user_groups(socket);

                std::cout << "hope the groups sent\n";

                send_mdm(socket);
                break;
            }
            case 6:
            {
                std::cout << "sending groups\n";

                send_user_groups(socket);

                std::cout << "hope the groups sent\n";

                std::cout << "sening mdms\n";
                show_mdms(socket);
                std::cout << "Hopefully he ot it \n";
                break;
            }
            case 7:
                std::cout << "User quit\n";
                did_user_quit = true;
                break;
            default:
                break;
            }
        }
        catch (const std::invalid_argument &error)
        {
            std::cerr << "Integer invalit " << error.what() << '\n';
            continue;
        }
        catch (const std::out_of_range &error)
        {
            std::cerr << "Integer out of range " << error.what() << '\n';
            continue;
        }
    }
}

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
        std::this_thread::sleep_for(std::chrono::seconds(60));

        std::cout << "Checking Sockets..\n";

        std::lock_guard<std::mutex> lock(sockets_mutex);
        std::cout << "Searching vec..\n";
        for (auto it = sockets.begin(); it != sockets.end();)
        {
            boost::system::error_code ec;
            char buffer[1];
            size_t len = (*it)->read_some(boost::asio::buffer(buffer, 1), ec);

            if (ec == boost::asio::error::would_block || ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset)
            {
                std::cout << "Deleting a socket...\n";
                std::string ip = (*it)->remote_endpoint().address().to_string();
                delete_user(ip);
                it = sockets.erase(it);
                std::cout << "Removed a dead socket.\n";
                continue;
            }
            ++it;
        }
    }
}

void server::start_monitoring()
{
    std::cout << "MONITORING STARTED\n";
    std::thread([this]()
                {
        this->monitor_user_connections();
        checker_io_context.run(); })
        .detach();
}

void server::add_id_to_list(int id_user, std::string ip)
{
    std::cout << "Adding User to vector\n";
    this->users.push_back(std::make_pair(id_user, ip));
}

void server::login(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    check_socket(*socket);
    std::cout << "waiting for user to send data\n";
    std::string received_data = get_data(*socket);
    std::cout << "Got the un and pas\n";
    try
    {
        auto received_json = nlohmann::json::parse(received_data);
        std::cout << "received json:\t" << received_json << '\n';
        std::string username = received_json["username"];
        std::string password = received_json["password"];

        std::cout << "Parsed Json\n";

        int id_user = server_sql->get_id_from_username(username);
        if (id_user != -1)
        {
            std::string password_in_db = server_sql->get_user_password(id_user);
            if (password_in_db == password)
            {
                check_socket(*socket);

                std::string suc = "Log successful \n";
                send_data(*socket, suc);

                std::string ip = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());

                add_id_to_list(id_user, ip);
                std::cout << "Adding sock\n";
                sockets.push_back(socket);

                std::cout << "Going Next\n";
                next_up(socket);
            }
            else
            {
                check_socket(*socket);

                send_data(*socket, "Incorrect password.\n");
            }
        }
        else
        {
            check_socket(*socket);

            send_data(*socket, "Incorrect username.\n");
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        check_socket(*socket);

        send_data(*socket, "JSON Parsing Error: " + std::string(e.what()) + "\n");
    }
}

void server::signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    check_socket(*socket);

    std::cout << "waiting for user to send data\n";
    std::string received_data = get_data(*socket);
    std::cout << "Got the un and pas\n";

    try
    {
        auto received_json = nlohmann::json::parse(received_data);
        std::string username = received_json["username"];
        std::string password = received_json["password"];
        int id_user = server_sql->get_id_from_username(username);
        if (id_user == -1)
        {
            check_socket(*socket);

            server_sql->add_username_and_password(username, password);
            std::string suc = "Signup successful , now restart and log in\n";
            send_data(*socket, suc);
        }
        else
        {
            check_socket(*socket);

            send_data(*socket, "Username already exists.\n");
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        check_socket(*socket);

        send_data(*socket, "JSON Parsing Error: " + std::string(e.what()) + "\n");
    }
}

void server::login_or_signup(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    check_socket(*socket);

    std::cout << "Waiting fo com,mand\n";
    std::string received_data = get_data(*socket);

    try
    {
        std::cout << "Got a command\n";
        auto received_json = nlohmann::json::parse(received_data);
        std::string command = received_json.value("command", "");

        if (command == "Login")
        {
            check_socket(*socket);

            std::cout << "Login\n";
            login(socket);
        }
        else if (command == "Signup")
        {
            check_socket(*socket);

            std::cout << "Signup";
            signup(socket);
        }
        else if (command == "Quit")
        {
            check_socket(*socket);

            std::cout << "User quit.\n";

            check_socket(*socket);
        }
        else
        {
            check_socket(*socket);

            send_data(*socket, "Invalid command. Try again.\n");
            login_or_signup(socket);
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        check_socket(*socket);

        send_data(*socket, "JSON Parsing Error: " + std::string(e.what()) + '\n');
    }
}

void server::acceptor_start()
{
    while (true)
    {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
        acceptor_.accept(*socket);
        std::thread(&server::login_or_signup, this, socket).detach();
    }
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

/*
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
*/

server::~server()
{
    if (this->server_sql != nullptr)
    {
        delete this->server_sql;
        this->server_sql = nullptr;
    }
}