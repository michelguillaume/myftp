/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** No file there, just an epitech header example.
** You can even have multiple lines if you want!
*/

#ifndef MYFTP_H
    #define MYFTP_H

    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <errno.h>
    #include <stdbool.h>
    #include <signal.h>
    #include <string.h>
    #include <limits.h>

    #include <poll.h>
    #include <netinet/in.h>

    #ifdef __APPLE__
        #include <sys/syslimits.h>
    #endif

    #ifdef EXIT_FAILURE
        #undef EXIT_FAILURE
        #define EXIT_FAILURE 84
    #endif

    #define SUCCESS 0
    #define FAILURE 1

typedef int socket_t;

    #define SOCKET_ERROR (-1)
    #define INVALID_SOCKET (-1)

    #define USERNAME_SIZE 256

typedef enum {
    NOT_AUTH,
    WAIT_PASS,
    AUTH,
    QUIT
} state_t;

typedef struct {
    state_t state;
    char username[USERNAME_SIZE];
    char pwd[PATH_MAX];
} user_data_t;

typedef struct {
    socket_t socket;
    struct sockaddr_in address;
    char *sending_buffer;
    char *receiving_buffer;
    user_data_t user_data;
} peer_t;

//typedef struct server_data {
//    TAILQ_HEAD(team_head, team) team_list;
//    struct user_head user_list;
//} server_data_t;

typedef struct {
    socket_t listen_sock;
    uint16_t port;
    char *path;
    peer_t *connection_list;
    struct pollfd *pfds;
    //server_data_t server_data;
} server_t;

typedef struct {
    char *str;
    void (*ptr)(server_t *, char *, peer_t *);
} fnct_ptr_t;

_Noreturn void server_loop(server_t *srv);
void process_command(server_t *srv, peer_t *conn);
void close_client_connection(server_t *srv, peer_t *conn, int idx);
void handle_new_connection(server_t *srv);

void my_user(server_t *srv, char *arg, peer_t *conn);
void my_pass(server_t *srv, char *arg, peer_t *conn);
void my_noop(server_t *, char *, peer_t *conn);
void my_help(server_t *, char *, peer_t *conn);
void my_pwd(server_t *srv, char *arg, peer_t *conn);
void my_cwd(server_t *srv, char *arg, peer_t *conn);

#endif
