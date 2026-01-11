#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#define MAX_PACKET 1024
#define HASH_SIZE 1024

volatile sig_atomic_t running = 1;

struct client_key {
    uint32_t ip;
    uint16_t port;
};

struct client_entry {
    struct client_key key;
    int counter;
    struct client_entry *next;
};

struct client_entry *table[HASH_SIZE];

unsigned hash_key(struct client_key k) {
    return (k.ip ^ k.port) % HASH_SIZE;
}

struct client_entry *find_client(struct client_key k) {
    unsigned h = hash_key(k);
    for (struct client_entry *e = table[h]; e; e = e->next)
        if (e->key.ip == k.ip && e->key.port == k.port)
            return e;
    return NULL;
}

struct client_entry *find_or_create(struct client_key k) {
    struct client_entry *e = find_client(k);
    if (e) return e;

    unsigned h = hash_key(k);
    e = calloc(1, sizeof(*e));
    e->key = k;
    e->counter = 0;
    e->next = table[h];
    table[h] = e;
    return e;
}

void remove_client(struct client_key k) {
    unsigned h = hash_key(k);
    struct client_entry **pp = &table[h];
    while (*pp) {
        if ((*pp)->key.ip == k.ip && (*pp)->key.port == k.port) {
            struct client_entry *tmp = *pp;
            *pp = tmp->next;
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

void cleanup(void) {
    for (int i = 0; i < HASH_SIZE; i++) {
        struct client_entry *e = table[i];
        while (e) {
            struct client_entry *n = e->next;
            free(e);
            e = n;
        }
        table[i] = NULL;
    }
}

void on_sig(int s) { (void)s; running = 0; }

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <port>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, on_sig);
    signal(SIGTERM, on_sig);

    int port = atoi(argv[1]);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }

    unsigned char buf[MAX_PACKET];

    while (running) {
        struct sockaddr_in cli;
        socklen_t cli_len = sizeof(cli);
        ssize_t n = recvfrom(sock, buf, sizeof(buf)-1, 0,
                             (struct sockaddr*)&cli, &cli_len);
        if (n <= 0) continue;
        buf[n] = 0;

        struct client_key key = { cli.sin_addr.s_addr, cli.sin_port };

        if (strcmp((char*)buf, "CLOSE") == 0) {
            printf("Client %s:%d disconnected, counter reset.\n",
                   inet_ntoa(*(struct in_addr*)&key.ip), ntohs(key.port));
            remove_client(key);
            continue;
        }

        struct client_entry *e = find_or_create(key);
        e->counter++;

        char reply[512];
        snprintf(reply, sizeof(reply), "%s %d", buf, e->counter);

        sendto(sock, reply, strlen(reply), 0,
               (struct sockaddr*)&cli, cli_len);
    }

    cleanup();
    close(sock);
    return 0;
}
