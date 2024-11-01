#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define PORT 8080

struct Subscriber
{
    int socket;
    std::string topic;
};

std::vector<Subscriber> subscribers;
std::mutex lock;

void handle_client(int client_socket);
void publish_message(const std::string &topic, const std::string &message);

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Broker server started on port " << PORT << std::endl;

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        std::thread client_thread(handle_client, new_socket);
        client_thread.detach();
    }

    return 0;
}

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);

    if (strncmp(buffer, "SUBSCRIBE", 9) == 0)
    {
        // Handle subscription
        std::string topic(buffer + 10);

        std::lock_guard<std::mutex> guard(lock);
        if (subscribers.size() < MAX_CLIENTS)
        {
            subscribers.push_back({client_socket, topic});
            std::cout << "New subscriber added to topic: " << topic << std::endl;
        }
        else
        {
            std::cout << "Max subscriber limit reached." << std::endl;
        }
    }
    else if (strncmp(buffer, "PUBLISH", 7) == 0)
    {
        // Handle publishing
        std::string topic, message;
        std::istringstream iss(buffer + 8);
        iss >> topic;
        std::getline(iss, message);
        publish_message(topic, message);
        close(client_socket);
    }
}

void publish_message(const std::string &topic, const std::string &message)
{
    std::cout << "Publishing message to topic '" << topic << "': " << message << std::endl;

    std::lock_guard<std::mutex> guard(lock);
    for (const auto &subscriber : subscribers)
    {
        if (subscriber.topic == topic)
        {
            send(subscriber.socket, message.c_str(), message.size(), 0);
        }
    }
}
