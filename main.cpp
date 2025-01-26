#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <thread>

#define PORT 8080

std::mutex log_mutex;

void log_message(const std::string &message)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << message << std::endl;
}

void handle_client(int client_socket)
{
    char buffer[1024] = {0};
    const char *response = "Server: Message received!";
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            log_message("Message received: " + std::string(buffer));

            send(client_socket, response, strlen(response), 0);
            log_message("Message sent to client: " + std::string(response));
        }
        else if (bytes_read == 0)
        {
            log_message("Client disconnected.");
            close(client_socket);
            break;
        }
        else
        {
            perror("Read error");
            close(client_socket);
            break;
        }
    }
}

int main()
{
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *response = "Message received by server";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (const sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        return -1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        return -1;
    }

    log_message("Server is listening on port " + std::to_string(PORT));

    std::vector<std::thread> client_threads;

    while (true)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            perror("Accept failed");
            return -1;
        }
        log_message("New client connected.");
        client_threads.emplace_back(handle_client, new_socket);
    }

    for (auto &thread : client_threads)
    {
        thread.join();
    }

    close(server_fd);

    return 0;
}
