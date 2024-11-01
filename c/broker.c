#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define PORT 8080

typedef struct
{
    int socket;
    char topic[50];
} Subscriber;

Subscriber subscribers[MAX_CLIENTS];
int subscriber_count = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *client_socket);
void publish_message(const char *topic, const char *message);

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
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
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Broker server started on port %d\n", PORT);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        pthread_t thread;
        int *client_socket = malloc(sizeof(int));
        *client_socket = new_socket;
        pthread_create(&thread, NULL, handle_client, client_socket);
    }

    return 0;
}

void *handle_client(void *client_socket)
{
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    free(client_socket);

    read(sock, buffer, BUFFER_SIZE);
    if (strncmp(buffer, "SUBSCRIBE", 9) == 0)
    {
        // Handle subscription
        char topic[50];
        sscanf(buffer + 10, "%s", topic);

        pthread_mutex_lock(&lock);
        if (subscriber_count < MAX_CLIENTS)
        {
            subscribers[subscriber_count].socket = sock;
            strcpy(subscribers[subscriber_count].topic, topic);
            subscriber_count++;
            printf("New subscriber added to topic: %s\n", topic);
        }
        else
        {
            printf("Max subscriber limit reached.\n");
        }
        pthread_mutex_unlock(&lock);
    }
    else if (strncmp(buffer, "PUBLISH", 7) == 0)
    {
        // Handle publishing
        char topic[50], message[BUFFER_SIZE];
        sscanf(buffer + 8, "%s %[^\n]", topic, message);
        publish_message(topic, message);
        close(sock);
    }

    return NULL;
}

void publish_message(const char *topic, const char *message)
{
    printf("Publishing message to topic '%s': %s\n", topic, message);

    pthread_mutex_lock(&lock);
    for (int i = 0; i < subscriber_count; i++)
    {
        if (strcmp(subscribers[i].topic, topic) == 0)
        {
            send(subscribers[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}
