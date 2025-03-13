#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  // For close() function
#include <arpa/inet.h> // For inet_ntoa

#define MAX 80
#define PORT 8088
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    socklen_t clen;
    struct sockaddr_in cli;
    clen = sizeof(cli);

    for (;;) {
        bzero(buff, MAX);  // Clear the buffer
        // Receive message from client
        ssize_t recv_len = recvfrom(sockfd, buff, MAX - 1, 0, (SA *)&cli, &clen);
        if (recv_len < 0) {
            perror("recvfrom failed");
            exit(1);
        }
        buff[recv_len] = '\0'; // Null-terminate received data
        printf("From client: %s\n", buff);

        // Print the client IP address for debugging
        char *client_ip = inet_ntoa(cli.sin_addr);  // Correct usage of inet_ntoa
        if (client_ip != NULL) {  // Ensure inet_ntoa returned a valid pointer
            printf("Message received from IP: %s\n", client_ip);
        } else {
            printf("Failed to convert client IP address.\n");
        }

        // Get input from server and send to client
        bzero(buff, MAX);  // Clear the buffer
        printf("Enter message: ");
        if (fgets(buff, MAX, stdin) == NULL) {
            perror("fgets failed");
            break; // Exit loop on error
        }

        size_t len = strlen(buff);
        if (len > 0 && buff[len - 1] == '\n') {
            buff[len - 1] = '\0'; // Remove trailing newline
        }

        sendto(sockfd, buff, strlen(buff), 0, (SA *)&cli, clen);

        // Exit if message contains "exit"
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on all interfaces
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        perror("Socket bind failed");
        exit(1);
    } else {
        printf("Socket successfully binded..\n");
    }

    func(sockfd);  // Function for handling communication
    close(sockfd); // Close socket
    return 0;
}

