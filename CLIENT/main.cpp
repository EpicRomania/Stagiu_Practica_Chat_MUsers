///#include "client.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

int main() {
    bool was_warning_sent=false;


    boost::asio::io_context io_context;

    
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "8443");

    
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    
    std::cout << "Choose:\n1)Login\n2)Signup\n3)Quit\n ";
    
    int option;
    std::cin>>option;
    std::string command;

    if(option==1)
    {
        command = "Login";
    }
    else if(option == 2)
    {
        command = "Signup";
    }
    else if(option == 3)
    {
        command = "Quit";
    }


    nlohmann::json j;
    j["command"] = command;

    std::string request = j.dump() + "\n";

   
    boost::asio::write(socket, boost::asio::buffer(request));


   
        boost::system::error_code error;

        if(!was_warning_sent)
        {   
            std::array<char, 256> buffer;
            

            std::size_t length = socket.read_some(boost::asio::buffer(buffer), error);
            if (error) {
            return -1;
            }

            std::string message(buffer.data(), length); 
            std::cout << "\n Message from server: " << message << std::endl;
        }
        std::cout << "\n Enter your Username; \t";
        std::string username;
        std::cin >> username;


        std::cout << "\n Enter your Password; \t";
        std::string password;
        std::cin >> password;

        nlohmann::json login_signup_details;
        login_signup_details["username"] = username;
        login_signup_details["password"] = password;

        std::string details = login_signup_details.dump() + "\n";

        boost::asio::write(socket, boost::asio::buffer(details));

        std::array<char, 256> buffer_response; // Create a buffer to hold incoming data

        std::size_t length_response = socket.read_some(boost::asio::buffer(buffer_response), error);
        if (error) {
           return -1;
        }

        std::string response(buffer_response.data(), length_response); // Convert the buffer to a string
        std::cout << "\n Response from server: " << response << '\n';
        socket.close();
        return 0;
    }
  

