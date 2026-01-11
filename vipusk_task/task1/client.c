#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_PACKET 1024

int sock;
struct sockaddr_in srv;

void send_close(int sig) {
    (void)sig;
    const char *msg = "CLOSE";
    sendto(sock, msg, strlen(msg), 0,
           (struct sockaddr*)&srv, sizeof(srv));
    close(sock);
    printf("\nSent CLOSE to server. Exiting.\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    const char *srv_ip = argv[1];
    int srv_port = atoi(argv[2]);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(srv_port);
    if (inet_pton(AF_INET, srv_ip, &srv.sin_addr) <= 0) {
        perror("inet_pton"); return 1;
    }

    signal(SIGINT, send_close);
    signal(SIGTERM, send_close);

    char line[256];
    unsigned char buf[MAX_PACKET];

    while (1) {
        printf("> "); fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        sendto(sock, line, strlen(line), 0,
               (struct sockaddr*)&srv, sizeof(srv));

        if (strcmp(line, "CLOSE") == 0) break;

        socklen_t srv_len = sizeof(srv);
        ssize_t n = recvfrom(sock, buf, sizeof(buf)-1, 0,
                             (struct sockaddr*)&srv, &srv_len);
        if (n <= 0) continue;
        buf[n] = 0;

        printf("reply: %s\n", buf);
    }

    // При нормальном завершении отправляем CLOSE
    send_close(0);
    return 0;
}
