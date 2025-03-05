/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_pasv.c
*/

#include "myftp.h"
#include "cvector.h"

static void send_error(peer_t *conn, const char *msg, int len)
{
    if (vector_push_back(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

static int verify_auth(peer_t *conn)
{
    if (conn->user_data.state != AUTH) {
        send_error(conn, "530 Not logged in.\r\n", 20);
        return FAILURE;
    }
    return SUCCESS;
}

static int verify_args(const char *arg, peer_t *conn)
{
    if (arg && arg[0] != '\0') {
        send_error(conn, "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

void my_pasv(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;

}
