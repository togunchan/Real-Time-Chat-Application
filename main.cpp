#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
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

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        return -1;
    }

    int bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        std::cout << "Message received: " << buffer << std::endl;

        send(new_socket, response, strlen(response), 0);
        std::cout << "Message sent to client." << std::endl;
    }
    else if (bytes_read == 0)
    {
        std::cout << "Client disconnected." << std::endl;
        close(new_socket);
    }
    else
    {
        perror("Read error");
        close(new_socket);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}
