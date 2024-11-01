#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void publish(const std::string &topic, const std::string &message);

int main()
{
    publish("sports", "Sports update: Team A won!");
    return 0;
}

void publish(const std::string &topic, const std::string &message)
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
        return;
    }

    // Connecting to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        return;
    }

    // Formatting the publish message
    snprintf(buffer, BUFFER_SIZE, "PUBLISH %s %s", topic.c_str(), message.c_str());

    // Sending the message to the server
    send(sock, buffer, strlen(buffer), 0);

    // Closing the socket
    close(sock);
}
