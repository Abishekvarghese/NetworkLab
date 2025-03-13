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
    struct sockaddr_in serverAddr, newAddr;
    socklen_t addr_size;
    int frame_id = 0;
    Frame frame_recv, frame_send;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    addr_size = sizeof(newAddr);

    while (1) {
        int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(Frame), 0, (struct sockaddr*)&newAddr, &addr_size);
        if (f_recv_size > 0 && frame_recv.frame_kind == 1 && frame_recv.sq_no == frame_id) {
            printf("[+] Frame Received: %s\n", frame_recv.packet.data);

            // Prepare the acknowledgment frame
            frame_send.frame_kind = 2;  // Acknowledgment kind
            frame_send.sq_no = frame_recv.sq_no + 1;
            frame_send.ack = 1;
            strcpy(frame_send.packet.data, "Acknowledgment");

            // Send acknowledgment
            sendto(sockfd, &frame_send, sizeof(frame_send), 0, (struct sockaddr*)&newAddr, addr_size);
            printf("[+] Ack Sent\n");
        } else {
            printf("[-] Frame Not Received\n");
        }

        frame_id++;  // Increment frame_id for the next frame
    }

    close(sockfd);
    return 0;
}

