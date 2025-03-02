/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** No file there, just an epitech header example.
** You can even have multiple lines if you want!
*/

#ifndef MYFTP_H
#define MYFTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <poll.h>

#ifdef EXIT_FAILURE
#undef EXIT_FAILURE
#define EXIT_FAILURE 84
#endif

#define SUCCESS 0
#define FAILURE 1

//typedef struct user {
//    char name[MAX_NAME_LENGTH];
//    uuid_t user_uuid;
//    int current_connections;
//} user_t;

typedef int socket_t;

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)

typedef struct {
    socket_t socket;
    struct sockaddr_in address;
    char *sending_buffer;
    char *receiving_buffer;
    //user_t *user;
} peer_t;

//typedef struct server_data {
//    TAILQ_HEAD(team_head, team) team_list;
//    struct user_head user_list;
//} server_data_t;

typedef struct {
    socket_t listen_sock;
    uint16_t port;
    peer_t *connection_list;
    //server_data_t server_data;
} server_t;

_Noreturn void server_loop(server_t *my_server);

#endif
