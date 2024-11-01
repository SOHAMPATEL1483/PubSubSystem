#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void subscribe(const char *topic);

int main()
{
    subscribe("sports");
    return 0;
}

void subscribe(const char *topic)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address\n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection failed\n");
        return;
    }

    snprintf(buffer, BUFFER_SIZE, "SUBSCRIBE %s", topic);
    send(sock, buffer, strlen(buffer), 0);

    // Receive messages
    while (1)
    {
        int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0)
            break;

        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
    }

    close(sock);
}
