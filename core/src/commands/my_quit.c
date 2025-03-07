/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_quit.c
*/

#include "myftp.h"
#include "cvector.h"

static void send_message(peer_t *conn, const char *msg, int len)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

void my_quit(server_t *, char *, peer_t *conn)
{
    send_message(conn, "221 Service closing control connection.\r\n", 41);
    conn->should_quit = true;
}
