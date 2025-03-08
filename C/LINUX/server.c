#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8080
#define MAX_ROOMS 100
#define MAX_CLIENTS_PER_ROOM 10
#define THREAD_POOL_SIZE sysconf(_SC_NPROCESSORS_ONLN)
#define QUEUE_SIZE 1024

typedef struct {
    int rank;  // 0-12 (2 through Ace)
    int suit;  // 0-3 (Spades, Clubs, Diamonds, Hearts)
} Card;

typedef struct {
    char name[32];
    Card* cards;
    int card_count;
    int client_fd;
    SSL* ssl;
} Player;

typedef struct {
    char code[7];  // 6-character room code + null terminator
    Player* players;
    int player_count;
    int max_players;
    Card deck[52];
    int deck_size;
    int started;
} GameRoom;

const char* RANKS[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
const char* SUITS[] = {"Spades", "Clubs", "Diamonds", "Hearts"};

GameRoom* rooms = NULL;
int room_count = 0;
pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int client_fd;
    SSL* ssl;
    int in_room;  // -1 if not in room, otherwise index of room
} connection_t;

connection_t task_queue[QUEUE_SIZE];
int task_count = 0;
pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;
pthread_t thread_pool[THREAD_POOL_SIZE];
SSL_CTX* ssl_ctx;

void generate_room_code(char* code) {
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 6; i++) {
        code[i] = chars[rand() % 36];
    }
    code[6] = '\0';
}

int find_room_by_code(const char* code) {
    for (int i = 0; i < room_count; i++) {
        if (strcmp(rooms[i].code, code) == 0) return i;
    }
    return -1;
}

void send_to_client(SSL* ssl, const char* msg) {
    SSL_write(ssl, msg, strlen(msg));
}

void shuffle_deck(Card* deck, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void init_deck(GameRoom* room) {
    room->deck_size = 52;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            room->deck[suit * 13 + rank] = (Card){rank, suit};
        }
    }
    shuffle_deck(room->deck, room->deck_size);
}

void distribute_cards(GameRoom* room) {
    int cards_per_player = room->deck_size / room->player_count;
    for (int i = 0; i < room->player_count; i++) {
        room->players[i].cards = malloc(cards_per_player * sizeof(Card));
        room->players[i].card_count = cards_per_player;
        memcpy(room->players[i].cards, &room->deck[i * cards_per_player], cards_per_player * sizeof(Card));
    }
    room->deck_size = 0;
}

void send_player_cards(Player* player) {
    char buffer[1024] = "CARDS:\n";
    for (int i = 0; i < player->card_count; i++) {
        char card_str[32];
        snprintf(card_str, sizeof(card_str), "%d) %s of %s\n", i + 1, RANKS[player->cards[i].rank], SUITS[player->cards[i].suit]);
        strcat(buffer, card_str);
    }
    send_to_client(player->ssl, buffer);
}

void broadcast_room(GameRoom* room, const char* msg) {
    for (int i = 0; i < room->player_count; i++) {
        send_to_client(room->players[i].ssl, msg);
    }
}

void* worker_thread(void* arg) {
    while (1) {
        connection_t task;
        pthread_mutex_lock(&task_mutex);
        while (task_count == 0) pthread_cond_wait(&task_cond, &task_mutex);
        task = task_queue[--task_count];
        pthread_mutex_unlock(&task_mutex);

        char buffer[16384];
        int bytes = SSL_read(task.ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            SSL_shutdown(task.ssl);
            SSL_free(task.ssl);
            close(task.client_fd);
            continue;
        }
        buffer[bytes] = '\0';
        printf("Received from %d: %s\n", task.client_fd, buffer);

        pthread_mutex_lock(&rooms_mutex);
        if (task.in_room == -1) {  // Lobby commands
            if (strncmp(buffer, "NAME:", 5) == 0) {
                // Store name temporarily or ignore until room join
            } else if (strncmp(buffer, "CREATE:", 7) == 0) {
                int max_players = atoi(buffer + 7);
                if (max_players < 4 || max_players > MAX_CLIENTS_PER_ROOM) max_players = 4;
                rooms = realloc(rooms, (room_count + 1) * sizeof(GameRoom));
                GameRoom* room = &rooms[room_count++];
                generate_room_code(room->code);
                room->players = malloc(max_players * sizeof(Player));
                room->player_count = 1;
                room->max_players = max_players;
                room->started = 0;
                strncpy(room->players[0].name, "Player", 32);  // Placeholder, update with real name if needed
                room->players[0].client_fd = task.client_fd;
                room->players[0].ssl = task.ssl;
                room->players[0].card_count = 0;
                room->players[0].cards = NULL;
                task.in_room = room_count - 1;
                char response[64];
                snprintf(response, sizeof(response), "ROOM:%s", room->code);
                send_to_client(task.ssl, response);
            } else if (strncmp(buffer, "JOIN:", 5) == 0) {
                char* code = buffer + 5;
                int room_idx = find_room_by_code(code);
                if (room_idx >= 0 && rooms[room_idx].player_count < rooms[room_idx].max_players && !rooms[room_idx].started) {
                    Player* p = &rooms[room_idx].players[rooms[room_idx].player_count++];
                    strncpy(p->name, "Player", 32);  // Update with real name if provided
                    p->client_fd = task.client_fd;
                    p->ssl = task.ssl;
                    p->card_count = 0;
                    p->cards = NULL;
                    task.in_room = room_idx;
                    send_to_client(task.ssl, "JOINED");
                    char join_msg[64];
                    snprintf(join_msg, sizeof(join_msg), "Player joined (%d/%d)", rooms[room_idx].player_count, rooms[room_idx].max_players);
                    broadcast_room(&rooms[room_idx], join_msg);
                } else {
                    send_to_client(task.ssl, "ERROR:Invalid or full room");
                }
            } else if (strcmp(buffer, "LIST") == 0) {
                char list[1024] = "ROOMS:\n";
                for (int i = 0; i < room_count; i++) {
                    if (!rooms[i].started && rooms[i].player_count < rooms[i].max_players) {
                        char room_info[64];
                        snprintf(room_info, sizeof(room_info), "%s (%d/%d)\n", rooms[i].code, rooms[i].player_count, rooms[i].max_players);
                        strcat(list, room_info);
                    }
                }
                send_to_client(task.ssl, list);
            } else if (strcmp(buffer, "START") == 0 && task.in_room >= 0) {
                GameRoom* room = &rooms[task.in_room];
                if (room->player_count >= 2) {  // Minimum 2 players
                    room->started = 1;
                    init_deck(room);
                    distribute_cards(room);
                    for (int i = 0; i < room->player_count; i++) {
                        send_player_cards(&room->players[i]);
                    }
                    broadcast_room(room, "Game starting!\n");
                }
            }
        } else {  // In-game commands
            GameRoom* room = &rooms[task.in_room];
            if (strncmp(buffer, "CARD:", 5) == 0) {
                int choice = atoi(buffer + 5) - 1;
                // Implement game logic here (to be expanded)
            }
        }
        pthread_mutex_unlock(&rooms_mutex);
    }
    return NULL;
}

void init_thread_pool() {
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
    }
}

void handle_clients(int server_sock, SSL_CTX* ctx) {
    int epoll_fd = epoll_create1(0);
    struct epoll_event ev = { .events = EPOLLIN, .data.fd = server_sock };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &ev);
    struct epoll_event events[MAX_ROOMS * MAX_CLIENTS_PER_ROOM];
    init_thread_pool();

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_ROOMS * MAX_CLIENTS_PER_ROOM, 10);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_sock) {
                int client_fd = accept(server_sock, NULL, NULL);
                fcntl(client_fd, F_SETFL, O_NONBLOCK);
                SSL* ssl = ssl_accept(client_fd, ctx);
                if (ssl) {
                    connection_t conn = {client_fd, ssl, -1};
                    add_task(conn);
                    send_to_client(ssl, "Welcome! Use CREATE:<max_players>, JOIN:<code>, or LIST");
                }
            }
        }
    }
    close(epoll_fd);
}

// Rest of the server functions (create_server_socket, etc.) remain similar
int main() {
    printf("Server Starting\n");
    srand(time(NULL));
    int server_sock = create_server_socket();
    ssl_ctx = init_ssl_context();
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    fork_workers(server_sock, num_cores, ssl_ctx);
    while (1) sleep(1);
    SSL_CTX_free(ssl_ctx);
    close(server_sock);
    return 0;
}