#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void publish(const char *topic, const char *message);

int main()
{
    publish("sports", "Sports update: Team A won!");
    return 0;
}

void publish(const char *topic, const char *message)
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

    snprintf(buffer, BUFFER_SIZE, "PUBLISH %s %s", topic, message);
    send(sock, buffer, strlen(buffer), 0);
    close(sock);
}
