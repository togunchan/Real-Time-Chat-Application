#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <fcntl.h>

#define PORT 8080

std::mutex log_mutex;
std::vector<int> clients;
std::mutex client_mutex;
std::ofstream log_file;

void log_message(const std::string &message)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_file.is_open())
    {
        log_file << message << std::endl;
        std::cout << message << std::endl;
    }
    else
    {
        std::cerr << "Log file could not be opened!" << std::endl;
    }
}

void broadcast_message(const std::string &message, int sender_socket)
{
    std::vector<int> client_copy;
    std::vector<int> disconnected_clients;
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        client_copy = clients;
    }

    for (int client_socket : client_copy)
    {
        if (client_socket != sender_socket)
        {
            ssize_t bytes_send = send(client_socket, message.c_str(), message.length(), 0);
            if (bytes_send == -1)
            {
                log_message("Client " + std::to_string(client_socket) + " disconnected (send error).");
                disconnected_clients.push_back(client_socket);
            }
        }
    }

    if (!disconnected_clients.empty())
    {
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            for (int client_socket : disconnected_clients)
            {
                clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            }
        }

        for (int client_socket : disconnected_clients)
        {
            close(client_socket);
        }
    }
}

void handle_client(int client_socket)
{
    // set the client socket to non-blocking
    int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1)
        flags = 0;
    fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

    char buffer[1024] = {0};
    // std::string response_str = "Message received from the client with the socket ID " + std::to_string(client_socket) + "!";
    // const char *response = response_str.c_str();

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        clients.push_back(client_socket);
    }

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            std::string message = "Client " + std::to_string(client_socket) + ": " + std::string(buffer);
            log_message(message);
            broadcast_message(message, client_socket);
            // log_message(std::string(response) + ": " + std::string(buffer));
        }
        else if (bytes_read == 0)
        {
            log_message("Client disconnected.");
            // close(client_socket);
            break;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // no data available to read
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            else
            {
                log_message("Read error on client " + std::to_string(client_socket));
                break;
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    }

    close(client_socket);
}

bool initialize_log(const std::string &path)
{
    log_file.open(path, std::ios::app);
    if (!log_file.is_open())
    {
        std::cerr << "Failed to open log file!" << std::endl;
        return false;
    }
    return true;
}

int create_server_socket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket failed");
    }
    return server_fd;
}

bool configure_socket_options(int server_fd)
{
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        return false;
    }
    return true;
}

sockaddr_in create_server_address()
{
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    return address;
}

bool bind_server_address(int server_fd, const sockaddr_in &address)
{
    if (bind(server_fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0)
    {
        perror("Bind failed");
        return false;
    }
    return true;
}

bool start_listening(int server_fd)
{
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        return false;
    }
    return true;
}

bool accept_clients(int server_fd, sockaddr_in &address, socklen_t addrlen)
{
    std::vector<std::thread> client_threads;
    while (true)
    {
        socklen_t client_len = addrlen;
        int new_socket = accept(server_fd, reinterpret_cast<sockaddr *>(&address), &client_len);

        if (new_socket < 0)
        {
            perror("Accept failed");
            break;
        }
        log_message("New client connected.");
        client_threads.emplace_back(handle_client, new_socket);
    }

    for (auto &thread : client_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return false;
}

int main()
{
    if (!initialize_log("server_logs.txt"))
    {
        return -1;
    }

    int server_fd = create_server_socket();
    if (server_fd == -1)
    {
        return -1;
    }

    if (!configure_socket_options(server_fd))
    {
        close(server_fd);
        return -1;
    }

    sockaddr_in address = create_server_address();
    socklen_t addrlen = sizeof(address);

    if (!bind_server_address(server_fd, address))
    {
        close(server_fd);
        return -1;
    }

    if (!start_listening(server_fd))
    {
        close(server_fd);
        return -1;
    }

    log_message("Server is listening on port " + std::to_string(PORT));

    bool accept_success = accept_clients(server_fd, address, addrlen);
    close(server_fd);

    return accept_success ? 0 : -1;
}
