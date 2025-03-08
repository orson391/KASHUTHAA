#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_EVENTS 10

void client_event_loop(int sock, SSL* ssl) {
    int epoll_fd = epoll_create1(0);
    struct epoll_event ev = { .events = EPOLLIN | EPOLLET, .data.fd = sock };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev);
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sock) {
                char buffer[16384];
                int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    printf("%s", buffer);
                    if (strstr(buffer, "Welcome")) {
                        printf("Options: CREATE:<max_players>, JOIN:<code>, LIST\n> ");
                    } else if (strncmp(buffer, "ROOM:", 5) == 0) {
                        printf("Created room with code: %s\n", buffer + 5);
                        printf("Type START to begin when ready\n> ");
                    } else if (strcmp(buffer, "JOINED") == 0) {
                        printf("Joined room successfully\nWaiting for game to start...\n");
                    } else if (strstr(buffer, "Game starting")) {
                        printf("Enter card number to play when prompted\n");
                    } else if (strstr(buffer, "CARDS:")) {
                        printf("Your turn> ");
                    }
                    char input[64];
                    fgets(input, sizeof(input), stdin);
                    input[strcspn(input, "\n")] = 0;
                    if (strlen(input) > 0) {
                        send_message(ssl, input, strlen(input));
                    }
                } else {
                    printf("Server disconnected\n");
                    close(epoll_fd);
                    return;
                }
            }
        }
    }
}

int main() {
    printf("Client Starting\n");
    int sock = create_client_socket();
    int epoll_fd = epoll_create1(0);
    struct epoll_event ev = { .events = EPOLLOUT, .data.fd = sock };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev);
    struct epoll_event events[1];
    int nfds = epoll_wait(epoll_fd, events, 1, -1);
    if (nfds > 0 && (events[0].events & EPOLLOUT)) {
        int err;
        socklen_t len = sizeof(err);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
        if (err != 0) {
            fprintf(stderr, "Connection failed: %s\n", strerror(err));
            close(sock);
            exit(1);
        }
    }
    close(epoll_fd);

    SSL_CTX* ctx = init_ssl_context();
    SSL* ssl = ssl_connect(sock, ctx);
    printf("Client connected successfully!\n");

    client_event_loop(sock, ssl);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);
    return 0;
}