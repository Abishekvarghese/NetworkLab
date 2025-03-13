#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  // For close() function

#define MAX 80
#define PORT 8088
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int n;
    socklen_t clen;
    struct sockaddr_in cli;
    clen = sizeof(cli);

    for (;;) {
        bzero(buff, MAX);
        // Receive message from client
        if (recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&cli, &clen) < 0) {
            perror("recvfrom failed");
            exit(1);
        }
        printf("From client: %s\n", buff);

        // Print the client IP address for debugging
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Message received from IP: %s\n", client_ip);

        // Get input from server and send to client
        bzero(buff, MAX);
        n = 0;
        while ((buff[n++] = getchar()) != '\n');  // Take input until newline
        sendto(sockfd, buff, sizeof(buff), 0, (SA *)&cli, clen);

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
        printf("Socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on all interfaces
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }

    func(sockfd);  // Function for handling communication
    close(sockfd); // Close socket
}

