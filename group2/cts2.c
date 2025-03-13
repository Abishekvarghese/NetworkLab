#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_addr
#include <errno.h>
#include <unistd.h>    // For close()

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in servaddr, local;
    char buffer[1024];
    socklen_t len;

    // Check if the required arguments are provided
    if (argc != 3) {
        printf("Usage: %s <server_addr> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize and bind the local address
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(6677);          // Local port number
    local.sin_addr.s_addr = INADDR_ANY;   // Bind to any local address

    if (bind(sockfd, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Initialize the server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));  // Server port number
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) { // Validate IP address
        perror("Invalid server address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Prepare the message to send
    strcpy(buffer, "TIME");

    // Send the message to the server
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error in sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive the response from the server
    len = sizeof(servaddr);
    memset(buffer, 0, sizeof(buffer));  // Clear the buffer before receiving data

    printf("Waiting for the server's response...\n");
    if ((n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&servaddr, &len)) < 0) {
        perror("Error in recvfrom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the received data
    buffer[n] = '\0';

    // Print the response
    printf("The current time is: %s", buffer);

    // Clean up
    close(sockfd);

    return 0;
}

