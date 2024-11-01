#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void subscribe(const std::string &topic);

int main()
{
    subscribe("sports");
    return 0;
}

void subscribe(const std::string &topic)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converting IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address" << std::endl;
        close(sock);
        return;
    }

    // Connecting to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return;
    }

    // Formatting and sending the subscribe message
    std::string subscribe_message = "SUBSCRIBE " + topic;
    send(sock, subscribe_message.c_str(), subscribe_message.size(), 0);

    // Receiving messages in a loop
    while (true)
    {
        int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0)
            break;

        buffer[bytes_read] = '\0';
        std::cout << "Received message: " << buffer << std::endl;
    }

    // Closing the socket
    close(sock);
}
