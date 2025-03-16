#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int sockfd;  // Declare globally for signal handling

void handle_sigint(int sig) {
    printf("\nShutting down the server...\n");
    close(sockfd);  // Close the socket
    exit(EXIT_SUCCESS);  // Exit gracefully
}

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr, cliaddr;
    char buffer[128];  // Reduced buffer size
    time_t ct;
    socklen_t len;

    // Handle SIGINT (Ctrl+C)
    signal(SIGINT, handle_sigint);

    // Check command-line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));  // Convert port to network byte order
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %s...\n", argv[1]);

    while (1) {
        len = sizeof(cliaddr);
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer before reuse

        // Receive message from the client
        if (recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&cliaddr, &len) < 0) {
            perror("Error receiving data");
            continue;  // Continue serving other clients even if an error occurs
        }

        printf("Received message from client: %s\n", buffer);

        // Validate message (Only respond to "TIME")
        if (strcmp(buffer, "TIME") != 0) {
            printf("Invalid message from client: %s\n", buffer);
            continue;
        }

        // Get current time
        ct = time(NULL);
        snprintf(buffer, sizeof(buffer), "%s", ctime(&ct));

        // Send response back to the client
        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len) < 0) {
            perror("Error sending data");
        } else {
            printf("Sent time to client: %s", buffer);
        }
    }

    // Close the socket
    close(sockfd);
    return 0;
}

