#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#include "message.h"
#include "socket.h"

typedef struct {
    int socket_fd;
    char* name;
} connection_info;

void* receive_thread(void* arg) {
    connection_info* info = (connection_info*)arg;
    char* received;
    bool running = true;

    while (running) {
        received = receive_message(info->socket_fd);
        if (received == NULL) {
            break;
        }
        printf("\n%s: %s\n", info->name, received);
        
        if (strcmp(received, "quit") == 0) {
            running = false;
        }
        free(received);
    }

    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <my port> <other machine name> <other port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse arguments
    unsigned short my_port = atoi(argv[1]);
    char* other_name = argv[2];
    unsigned short other_port = atoi(argv[3]);

    // Open server socket
    int server_socket_fd = server_socket_open(&my_port);
    if (server_socket_fd == -1) {
        perror("Server socket was not opened");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_socket_fd, 1)) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %u\n", my_port);
    printf("Trying to connect to %s on port %u\n", other_name, other_port);

    // Try to connect to the other machine
    int client_socket_fd = -1;
    int server_conn_fd = -1;
    bool connected = false;
    
    // Try both connecting and accepting until one succeeds
    while (!connected) {
        // Try to connect
        client_socket_fd = socket_connect(other_name, other_port);
        if (client_socket_fd != -1) {
            printf("Successfully connected to other machine\n");
            // Accept connection from other side
            server_conn_fd = server_socket_accept(server_socket_fd);
            if (server_conn_fd == -1) {
                perror("accept failed after connect");
                close(client_socket_fd);
                exit(EXIT_FAILURE);
            }
            connected = true;
        } else {
            // If connect failed, try accepting
            server_conn_fd = server_socket_accept(server_socket_fd);
            if (server_conn_fd != -1) {
                printf("Accepted connection from other machine\n");
                // Connect back
                client_socket_fd = socket_connect(other_name, other_port);
                if (client_socket_fd == -1) {
                    perror("connect failed after accept");
                    close(server_conn_fd);
                    exit(EXIT_FAILURE);
                }
                connected = true;
            }
        }
        if (!connected) {
            usleep(100000); // Sleep for 100ms before retrying
        }
    }

    printf("Connected!\n");

    // Set up receiver thread
    pthread_t client_thread;
    connection_info client_info = {
        .socket_fd = server_conn_fd,
        .name = "Other"
    };
    
    if (pthread_create(&client_thread, NULL, receive_thread, &client_info) != 0) {
        perror("Failed to create receiver thread");
        exit(EXIT_FAILURE);
    }

    // Main loop for sending messages
    char message[MAX_MESSAGE_LENGTH];
    bool running = true;

    while (running) {
        // Get message from user
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Remove newline

        // Send the message
        if (send_message(client_socket_fd, message) == -1) {
            perror("Failed to send message");
            break;
        }

        if (strcmp(message, "quit") == 0) {
            running = false;
        }
    }

    // Clean up
    pthread_join(client_thread, NULL);
    close(server_conn_fd);
    close(client_socket_fd);
    close(server_socket_fd);

    return 0;
}