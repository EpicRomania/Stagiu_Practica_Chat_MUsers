#include "client.hpp"

void user::send_data(boost::asio::ip::tcp::socket &socket, const std::string &message)
{
    try
    {
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Send error: " << e.what() << std::endl;
    }
}

std::string user::get_data(boost::asio::ip::tcp::socket &socket)
{
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
void user::respone_next()
{
    std::string response = get_data(socket_);
    std::cout << response << '\n';
    return;
}

void user::receive_users()
{
    std::cout << "Active Users:\n";
    std::string data = get_data(socket_);

    if (!data.empty())
    {
        send_data(socket_, "OPA\n");
    }

    std::string::size_type start = 0;
    std::string::size_type end = data.find(" ] ");
    while (end != std::string::npos)
    {
        std::string user = data.substr(start, end - start);
        std::replace(user.begin(), user.end(), '|', ')');
        std::cout << user << '\n';
        start = end + 3;
        end = data.find(" ] ", start);
    }

    std::string lastUser = data.substr(start);
    std::replace(lastUser.begin(), lastUser.end(), '|', ')');
    std::cout << lastUser;

    send_data(socket_, "ACK\n");

    std::string rec_sz = get_data(socket_);
    std::cout << rec_sz << '\n';
}

void user::receive_user_groups()
{
    std::cout << "Groups you're a part of:\n";
    std::string data = get_data(socket_);
    /// std::cout << data;

    std::string::size_type start = 0;
    std::string::size_type end = data.find(" ] ");
    while (end != std::string::npos)
    {
        std::string group = data.substr(start, end - start);
        std::replace(group.begin(), group.end(), '|', ')');
        std::cout << group << '\n';
        start = end + 3;
        end = data.find(" ] ", start);
    }

    std::string lastGroup = data.substr(start);
    std::replace(lastGroup.begin(), lastGroup.end(), '|', ')');
    std::cout << lastGroup;
}

void user::dm_user()
{

    int usr_id = -999;
    std::cout << "Which user (ID):\t";
    std::cin >> usr_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// cin clear

    std::string usr_id_str = std::to_string(usr_id) + "\n";
    send_data(socket_, usr_id_str);

    std::string message;
    std::cout << "Message:\t";
    std::getline(std::cin, message);
    message += "\n";

    send_data(socket_, message);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear iar
}

void user::show_dms()
{
    std::cout << "waiting for data\n";
    std::string data = get_data(socket_);
    std::cout << "received data:\n";
    std::cout << data << '\n';
}

void user::create_group()
{
    std::string group_name;
    std::cout << "Choose a group name :\t";
    std::getline(std::cin, group_name);

    send_data(socket_, group_name);

    std::cout << "data sent hope the group was made\n";
}

void user::add_users_to_group()
{
    std::cout << "Choosing -1 will quit\n";
    nlohmann::json j;

    int user_id;
    int id_group;

    std::string user_id_str;
    std::string id_group_str;

    std::cout << "User ID:\t";
    std::cin >> user_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// cin clear

    if (user_id == -1)
    {
        j["command"] = "Quit";
        std::string quiting = j.dump();
        quiting += "\n";
        std::cout << quiting;
        send_data(socket_, quiting);

        return;
    }
    user_id_str = std::to_string(user_id);

    j["User_ID"] = user_id_str;

    std::cout << "Group ID:\t";
    std::cin >> id_group;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// cin clear

    if (id_group == -1)
    {
        j["command"] = "Quit";
        std::string quiting = j.dump();
        quiting += "\n";
        std::cout << quiting;

        send_data(socket_, quiting);

        return;
    }
    id_group_str = std::to_string(id_group);

    j["Group_ID"] = id_group_str;

    std::string data = j.dump();

    std::cout << "You chose the \nUser_ID:\t" << user_id_str << "\nGroup_ID:\t" << id_group_str << '\n';

    /// std::cout << data;

    send_data(socket_, data);
}

void user::send_mdm()
{
    std::cout << "Choosing -1 will quit\n";
    int group_id;
    std::cout << "Choose:\t";
    std::cin >> group_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// cin clear

    if (group_id == -1)
    {
        std::cout << "Quitting...\n";
        send_data(socket_, "QUIT\n");
        return;
    }

    std::string group_id_str = std::to_string(group_id);
    group_id_str += "\n";

    send_data(socket_, group_id_str);

    std::string message;
    std::cout << "Type your message:\t";
    std::getline(std::cin, message);
    message += "\n";

    send_data(socket_, message);
    std::cout << "message sent hopefully\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear iar
}

void user::show_mdms()
{
    std::cout << "Choosing -1 will quit\n";
    int group_id;
    std::cout << "Choose:\t";
    std::cin >> group_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// cin clear

    if (group_id == -1)
    {
        std::cout << "Quitting...\n";
        send_data(socket_, "QUIT\n");
        return;
    }

    std::string group_id_str = std::to_string(group_id);
    group_id_str += "\n";

    send_data(socket_, group_id_str);

    std::string mdms_received = get_data(socket_);

    std::cout << mdms_received;
}

void user::what_to_do()
{
    /// std::cout << "what do\n";
    bool did_user_quit = false;
    while (!did_user_quit)
    {
        std::string options = "Choose\n1)Dm someone \n2)Show DM's \n3)Create Group \n4)Add User to a group \n5)Message in a group \n6)Show Group DM's \n7)Quit\n";
        std::cout << options;

        int op;
        std::cout << "Your option :\t";
        std::cin >> op;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); /// clearing

        std::string op_s = std::to_string(op) + "\n";
        std::cout << "Option:\t" << op_s;

        if (op == 7)
        {
            send_data(socket_, op_s);
            did_user_quit = true;
            socket_.close();
            break;
        }
        else if (op > 7)
        {
            std::string oppa = get_data(socket_);
            std::cout << oppa << '\n';
            did_user_quit = false;
            break;
        }

        send_data(socket_, op_s);

        switch (op)
        {
        case 1:
        {
            std::cout << "-1)QUIT\n\n";
            receive_users();
            dm_user();
            break;
        }
        case 2:
            show_dms();
            break;
        case 3:
            create_group();
            break;
        case 4:
        {
            std::cout << "-1)QUIT";
            receive_users();

            receive_user_groups();

            add_users_to_group();
            break;
        }
        case 5:
        {
            std::cout << "-1)QUIT\n";

            receive_user_groups();

            std::cout << "User's group received?\n";

            send_mdm();

            break;
        }
        case 6:
        {
            std::cout << "-1)QUIT\n";

            receive_user_groups();

            show_mdms();
            break;
        }
        }
    }
}

void user::start_login_signup()
{
    std::string options = "Choose:\n1) Login\n2) Signup\n3) Quit\n";
    std::cout << options;

    int option;
    std::cin >> option;

    std::string command;
    switch (option)
    {
    case 1:
        command = "Login";
        break;
    case 2:
        command = "Signup";
        break;
    case 3:
        command = "Quit";
        send_data(socket_, command);
        socket_.close();
        return;
    default:
        std::cerr << "Invalid option. Exiting.\n";
        return;
    }

    nlohmann::json j;
    j["command"] = command;
    send_data(socket_, j.dump() + "\n");
    std::cout << "Option sent\n";

    std::cout << "Enter your Username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Enter your Password: ";
    std::string password;
    std::cin >> password;

    nlohmann::json data;
    data["username"] = username;
    data["password"] = password;

    std::string data_s = data.dump() + "\n";

    send_data(socket_, data_s);

    std::string response = get_data(socket_);
    std::cout << "Server: " << response << std::endl;

    if (response.find("successful") != std::string::npos)
    {
        what_to_do();
    }
    else
    {
        std::cout << "OOPs.." << std::endl;
        socket_.close();
    }
}

user::user(boost::asio::io_context &io_context) : io_context_(io_context), socket_(io_context)
{
    boost::asio::ip::tcp::resolver resolver(io_context_);

    auto endpoints = resolver.resolve("127.0.0.1", "8443");
    boost::asio::connect(socket_, endpoints);

    start_login_signup();
}

user::~user()
{
    socket_.close();
}