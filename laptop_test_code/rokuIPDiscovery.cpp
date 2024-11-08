#include <asio.hpp>
#include <iostream>

using asio::ip::udp;
using namespace std::chrono_literals;


std::string* ip = nullptr;

/**
 * This function sends an SSDP M-SEARCH request to search for Roku devices on the network.
 */
void sendSSDPRequestRoku(asio::io_context& io_context, udp::socket& socket, udp::endpoint& endpoint) {
    std::string request =
            "M-SEARCH * HTTP/1.1\r\n"
            "Host: 239.255.255.250:1900\r\n"
            "Man: \"ssdp:discover\"\r\n"
            "ST: roku:ecp\r\n"
            "MX: 3\r\n\r\n";

    socket.send_to(asio::buffer(request), endpoint);
    std::cout << "SSDP request sent.\n";
}

/**
 * Listens for responses from Roku devices for up to 5 seconds.
 */
bool listenForResponse(asio::io_context& io_context, udp::socket& socket, std::string*& response_ip) {
    char response[1024];
    udp::endpoint sender_endpoint;
    auto start_time = std::chrono::steady_clock::now();
    asio::steady_timer timer(io_context);

    bool responseReceived = false;
    while (std::chrono::steady_clock::now() - start_time < 5s) {  // Listen for 5 seconds
        timer.expires_after(500ms);  // Set a 500ms timeout

        // Create a variable to hold the response length
        size_t len = 0;

        // Run an async receive with a callback to capture response length
        socket.async_receive_from(asio::buffer(response), sender_endpoint,
                                  [&](const asio::error_code& error, std::size_t bytes_recvd) {
                                      if (!error) {
                                          len = bytes_recvd;
                                          responseReceived = true;
                                      }
                                  });

        // Wait for either the receive to complete or the timer to expire
        io_context.run_for(500ms);

        if (responseReceived) {
            std::string responseStr(response, len);

            auto locationPos = responseStr.find("LOCATION: ");
            if (locationPos != std::string::npos) {
                auto httpPos = responseStr.find("http://", locationPos);
                auto endPos = responseStr.find(":", httpPos + 7);
                auto rokuIP = responseStr.substr(httpPos + 7, endPos - (httpPos + 7));
                std::cout << "Response IP Address: " << rokuIP << std::endl;

                response_ip = new std::string (rokuIP);

                break;
            }
        }

        // Reset for next iteration
        responseReceived = false;
        io_context.restart();  // Reset the io_context for the next run
    }
    return responseReceived;
}

std::string* findRokuIP(){
    try {
        asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), 0));
        udp::endpoint multicast_endpoint(asio::ip::make_address("239.255.255.250"), 1900);

        sendSSDPRequestRoku(io_context, socket, multicast_endpoint);

        std::cout << "IP before: " + (ip == nullptr ? "empty" : *ip) + "\n\n";

        int count = 0;

        while (!listenForResponse(io_context, socket, ip) && count < 10) {
            std::cout << "IP while: " + (ip == nullptr ? "empty" : *ip) + "\n";
            std::cout << "No Roku found, trying again" << std::endl;

            count++;
        }

        std::cout << "Discovery complete.\n";
        std::cout << "\nIP after: " + (ip == nullptr ? "empty" : *ip) + "\n";

    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return ip;
}