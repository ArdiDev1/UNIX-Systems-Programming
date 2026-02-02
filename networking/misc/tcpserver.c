#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  // Create socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  // Set address properties
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
  address.sin_port = htons(PORT);

  // Bind socket to address
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Start listening for connections
  if (listen(server_fd, 3) < 0) {
    perror("Listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("TCP server listening on port %d...\n", PORT);

  // Accept a connection
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    perror("Accept");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Read message from client
  int valread = read(new_socket, buffer, BUFFER_SIZE);
  buffer[valread] = '\0';
  printf("Client: %s\n", buffer);

  // Send response
  char *response = "Hello from server";
  send(new_socket, response, strlen(response), 0);

  close(new_socket);
  close(server_fd);

  return 0;
}
