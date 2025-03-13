#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct packet {
    char data[1024];
} Packet;

typedef struct frame {
    int frame_kind, sq_no, ack;
    Packet packet;
} Frame;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    socklen_t addr_size;
    int frame_id = 0;
    Frame frame_send;
    Frame frame_recv;
    int ack_recv = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        if (ack_recv == 1) {
            frame_send.sq_no = frame_id;
            frame_send.frame_kind = 1;  // Send data frame
            frame_send.ack = 0;         // Not an acknowledgment
            printf("Enter data: ");
            scanf("%s", buffer);
            strcpy(frame_send.packet.data, buffer);

            // Send frame
            sendto(sockfd, &frame_send, sizeof(Frame), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            printf("[+] Frame Sent\n");

            addr_size = sizeof(serverAddr);
            int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(frame_recv), 0, (struct sockaddr*)&serverAddr, &addr_size);

            if (f_recv_size > 0 && frame_recv.sq_no == frame_id + 1 && frame_recv.ack == 1) {
                printf("[+] Ack Received\n");
                ack_recv = 1;
            } else {
                printf("[-] Ack Not Received\n");
                ack_recv = 0;
            }
        }

        frame_id++;  // Increment frame_id for the next frame
    }

    close(sockfd);
    return 0;
}

