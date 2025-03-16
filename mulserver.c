#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>  // For close()

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function to handle communication between server and client
void func(int sockfd, struct sockaddr_in *cliaddr, socklen_t len) {
    char buff[MAX];
    int n;

    for (;;) {
        bzero(buff, sizeof(buff));

        // Receive message from client
        n = recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)cliaddr, &len);
        if (n < 0) {
            printf("Failed to receive message\n");
            break;
        }

        printf("From client: %s\t To client: ", buff);

        bzero(buff, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n');  // Get server's response

        // Send response back to the client
        sendto(sockfd, buff, sizeof(buff), 0, (SA *)cliaddr, len);

        // If "exit" is typed, break the loop
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    // Setup server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any network interface
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }

    // Handle communication with client
    func(sockfd, &cliaddr, len);

    // Close the socket after communication ends
    close(sockfd);
}

