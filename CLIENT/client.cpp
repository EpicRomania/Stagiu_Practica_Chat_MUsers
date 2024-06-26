#include "client.hpp"

void user::what_to_do()
{
    std::cout << "what do\n";
}

void user::start_login_signup()
{
    std::cout << "Choose:\n1)Login\n2)Signup\n3)Quit\n ";
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
        break;
    default:
        std::cerr << "Invalid option. Exiting.\n";
        return;
    }

    nlohmann::json j;
    j["command"] = command;

    std::string request = j.dump() + "\n";
    boost::asio::write(socket_, boost::asio::buffer(request));

    if (option != 3)
    {
        boost::system::error_code error;
        std::array<char, 256> buffer_message;
        std::array<char, 256> buffer_response;

        std::size_t length = socket_.read_some(boost::asio::buffer(buffer_message), error);
        if (error)
        {
            std::cerr << "Error reading from socket: " << error.message() << '\n';
            return;
        }

        std::string message(buffer_message.data(), length);
        std::cout << "\nMessage from server: " << message << std::endl;

        std::cout << "\nEnter your Username: ";
        std::string username;
        std::cin >> username;

        std::cout << "\nEnter your Password: ";
        std::string password;
        std::cin >> password;

        nlohmann::json login_signup_details;
        login_signup_details["username"] = username;
        login_signup_details["password"] = password;

        request = login_signup_details.dump() + "\n";
        boost::asio::write(socket_, boost::asio::buffer(request));

        length = socket_.read_some(boost::asio::buffer(buffer_response), error);
        if (error)
        {
            std::cerr << "Error reading from socket: " << error.message() << '\n';
            return;
        }

        std::string message_response(buffer_response.data(), length);

        std::cout << "\nResponse from server: " << message_response << '\n';

        what_to_do();
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