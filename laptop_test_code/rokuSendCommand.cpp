#include <asio.hpp>
#include <iostream>
#include <string>

enum Mode {
    DIRECTIONAL,
    OK_BACK,
    VOL,
    NETFLIX_YOUTUBE
};

class RokuCommandHandler {
private:
    std::string roku_host;             // IP address of the Roku device
    asio::io_context io_context;       // io_context for managing I/O operations
    asio::steady_timer timer;          // Timer for adding delays between commands

    void sendCommand(const std::string& command) {
        try {
            asio::ip::tcp::resolver resolver(io_context);
            asio::ip::tcp::socket socket(io_context);

            // Resolve the host and port
            auto endpoints = resolver.resolve(roku_host, "8060");
            asio::connect(socket, endpoints);

            // Formulate the HTTP POST request
            std::string request = "POST " + command + " HTTP/1.1\r\n";
            request += "Host: " + roku_host + ":8060\r\n";
            request += "Connection: close\r\n\r\n";

            // Send the request
            asio::write(socket, asio::buffer(request));

            // Receive the response (optional, just to confirm the request was sent)
            asio::streambuf response;
            asio::read_until(socket, response, "\r\n");

            // Print the response status line
            std::istream response_stream(&response);
            std::string http_version;
            unsigned int status_code;
            std::string status_message;
            response_stream >> http_version >> status_code;
            std::getline(response_stream, status_message);

            if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
                std::cerr << "Invalid response\n";
                return;
            }

            std::cout << "Response status code: " << status_code << std::endl;
            timer.wait();
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

public:
    RokuCommandHandler(const std::string* ip)
            : roku_host(*ip), timer(io_context, std::chrono::milliseconds(300)) {}

    void sendUpCommand() { sendCommand("/keypress/up"); }
    void sendDownCommand() { sendCommand("/keypress/down"); }
    void sendLeftCommand() { sendCommand("/keypress/left"); }
    void sendRightCommand() { sendCommand("/keypress/right"); }
    void sendNetflixCommand() { sendCommand("/launch/12"); }
    void sendYouTubeCommand() { sendCommand("/launch/837"); }
    void sendOkCommand() { sendCommand("/keypress/select"); }
    void sendBackCommand() { sendCommand("/keypress/back"); }
    void sendVolumeUp() { sendCommand("/keypress/volumeup"); }
    void sendVolumeDown() { sendCommand("/keypress/volumedown"); }
    void sendHomeCommand() { sendCommand("/keypress/home"); }
};