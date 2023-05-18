#include "st_reactor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000

void handle_client(int client_fd) {
    char buffer[1024];
    int num_bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (num_bytes < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    } else if (num_bytes == 0) {
        printf("Client disconnected\n");
        close(client_fd);
    } else {
        printf("Received %d bytes from client: %.*s\n", num_bytes, num_bytes, buffer);
        send(client_fd, buffer, num_bytes, 0);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set server socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind server socket to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Create reactor and add server socket to it
    st_reactor_t *reactor = create_reactor();
    add_fd(reactor, server_fd, handle_client);

    // Start reactor
    start_reactor(reactor);

    // Wait for reactor to finish
    wait_for_reactor(reactor);

    // Clean up
    free(reactor);
    close(server_fd);

    return 0;
}
