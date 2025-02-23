#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080

void receive_message(int sock)
{
    char buffer[1024] = {0};
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_received < 0)
        {
            std::cerr << "Error in reading" << std::endl;
            break;
        }
        else if (bytes_received == 0)
        {
            std::cerr << "Server closed connection." << std::endl;
            break;
        }
        else
        {
            buffer[bytes_received] = '\0';
            std::cout << "\nServer: " << buffer << std::endl;
            std::cout << "Enter message: ";
            std::cout.flush();
        }
    }
}

int main()
{
    int sock = 0;
    struct sockaddr_in server_address;

    // create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    // Convert SERVER_ADDRESS to binary format and store it in server_address.sin_addr, checking its validity
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to server. Type messages and press Enter to send." << std::endl;

    // start the thread that will receive messages from the server
    std::thread receive_thread(receive_message, sock);

    std::string message;
    while (true)
    {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        if (message == "exit")
        {
            std::cout << "Disconnecting..." << std::endl;
            break;
        }
        // Send message to server
        send(sock, message.c_str(), message.length(), 0);
        std::cout << "Message sent" << std::endl;
    }

    close(sock);
    receive_thread.join();

    return 0;
}