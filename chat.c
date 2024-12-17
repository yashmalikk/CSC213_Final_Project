#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#include "message.h"
#include "socket.h"
#include "key_management.h"
#include "utility.h"


typedef struct {
    int socket_fd;
    char* name;
    PublicKeyPair peer_key;
    struct {
        long p;
        long q;
        long e;
        long phi;
        long modulus;
    } my_key;
} secure_connection_info;


void* receive_thread(void* arg) {
    secure_connection_info* info = (secure_connection_info*)arg;
    char* received;
    char decrypted[MAX_MESSAGE_LENGTH];
    bool running = true;

    while (running) {
        received = receive_message(info->socket_fd);
        if (received == NULL) {
            // Connection closed or error
            break;
        }

        // Decrypt using rsa_decrypt
        printf("\n %s", received); // TODO: earase later
        rsa_decrypt(received, decrypted, info->my_key.p, info->my_key.q, info->my_key.e);
        printf("\n%s: %s\n", info->name, decrypted);

        if (strcmp(decrypted, "quit") == 0) {
            running = false;
        }
        free(received);
    }

    return NULL;
}

bool exchange_keys(int socket_fd, PublicKeyPair* public_key, PublicKeyPair* peer_key) {
    char key_message[256];

    // Send our public key
    snprintf(key_message, sizeof(key_message), "%ld,%ld", public_key->public_key, public_key->modulus);
    if (send_message(socket_fd, key_message) == -1) {
        return false;
    }

    // Receive peer's public key
    char* received = receive_message(socket_fd);
    if (received == NULL) {
        return false;
    }

    // Parse received key
    if (sscanf(received, "%ld,%ld", &peer_key->public_key, &peer_key->modulus) != 2) {
        free(received);
        return false;
    }

    free(received);
    return true;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <my port> <other machine name> <other port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse arguments
    unsigned short my_port = (unsigned short)atoi(argv[1]);
    char* other_name = argv[2];
    unsigned short other_port = (unsigned short)atoi(argv[3]);

    // Initialize key management system
    if (!init_key_management()) {
        fprintf(stderr, "Failed to initialize key management system\n");
        exit(EXIT_FAILURE);
    }

    // Decide role based on ports
    bool i_am_client = (my_port < other_port);

    int conn_fd = -1;
    if (i_am_client) {
        // I'm client: connect to the other machine
        printf("Acting as client. Trying to connect to %s:%u...\n", other_name, other_port);
        conn_fd = socket_connect(other_name, other_port);
        if (conn_fd == -1) {
            perror("Failed to connect");
            exit(EXIT_FAILURE);
        }
        printf("Connected as client.\n");
    } else {
        // I'm server: set up listening and accept one connection
        printf("Acting as server. Listening on port %u...\n", my_port);

        int server_socket_fd = server_socket_open(&my_port);
        if (server_socket_fd == -1) {
            perror("Server socket was not opened");
            exit(EXIT_FAILURE);
        }

        if (listen(server_socket_fd, 1) != 0) {
            perror("listen failed");
            exit(EXIT_FAILURE);
        }

        printf("Waiting for incoming connection...\n");
        conn_fd = server_socket_accept(server_socket_fd);
        if (conn_fd == -1) {
            perror("accept failed");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
        printf("Accepted connection.\n");
        close(server_socket_fd);
    }

    // Set up secure connection info
    secure_connection_info connection = {
        .socket_fd = conn_fd,
        .name = "Other"
    };

    PublicKeyPair my_public_key;

    // Check if we have a stored key for this peer
    if (!has_stored_key(other_port)) {
        // Generate our keys
        twoPrimes(&connection.my_key.p, &connection.my_key.q);
        connection.my_key.modulus = connection.my_key.p * connection.my_key.q;
        connection.my_key.phi = (connection.my_key.p - 1) * (connection.my_key.q - 1);
        connection.my_key.e = 65537; // Common public exponent

        // Set up public key for exchange
        my_public_key.public_key = connection.my_key.e;
        my_public_key.modulus = connection.my_key.modulus;

        // Exchange keys with peer
        if (!exchange_keys(conn_fd, &my_public_key, &connection.peer_key)) {
            fprintf(stderr, "Failed to exchange keys\n");
            close(conn_fd);
            exit(EXIT_FAILURE);
        }

        // Store the peer's key
        if (!store_key(other_port, connection.peer_key)) {
            fprintf(stderr, "Failed to store peer's key\n");
            close(conn_fd);
            exit(EXIT_FAILURE);
        }
    } else {
        // Load the peer's stored key
        if (!load_key(other_port, &connection.peer_key)) {
            fprintf(stderr, "Failed to load peer's key\n");
            close(conn_fd);
            exit(EXIT_FAILURE);
        }

        // We generate our keys fresh each time (in a real scenario, we'd load our private key)
        twoPrimes(&connection.my_key.p, &connection.my_key.q);
        connection.my_key.modulus = connection.my_key.p * connection.my_key.q;
        connection.my_key.phi = (connection.my_key.p - 1) * (connection.my_key.q - 1);
        connection.my_key.e = 65537;
    }

    // Create receiver thread
    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, receive_thread, &connection) != 0) {
        perror("Failed to create receiver thread");
        close(conn_fd);
        exit(EXIT_FAILURE);
    }

    // Main message loop
    char message[MAX_MESSAGE_LENGTH];
    char encrypted[MAX_MESSAGE_LENGTH * 4];
    bool running = true;

    printf("You can now send messages. Type 'quit' to exit.\n");
    while (running) {
        if (fgets(message, sizeof(message), stdin) == NULL) {
            // EOF or error
            break;
        }
        message[strcspn(message, "\n")] = '\0'; // Remove newline

        // Encrypt message using peer's public key
        rsa_encrypt(message, encrypted, connection.peer_key.public_key,
                    connection.peer_key.modulus);

        // Send the encrypted message
        if (send_message(conn_fd, encrypted) == -1) {
            perror("Failed to send message");
            break;
        }

        if (strcmp(message, "quit") == 0) {
            running = false;
        }
    }

    // Clean up
    pthread_join(client_thread, NULL);
    close(conn_fd);

    return 0;
}
