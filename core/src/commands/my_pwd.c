/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_pwd.c
*/

#include "myftp.h"
#include "cvector.h"

static void send_error(peer_t *conn, const char *msg, int len)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
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

static int verify_args(char *arg, peer_t *conn)
{
    if (arg && arg[0] != '\0') {
        send_error(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static void send_pwd_response(peer_t *conn)
{
    char response[PATH_MAX + 32];

    snprintf(response, sizeof(response),
        "257 \"%s\" is current directory.\r\n", conn->user_data.pwd);
    if (VECTOR_PUSH_BACK(conn->sending_buffer, response, strlen(response))
        == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

void my_pwd(server_t *, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    send_pwd_response(conn);
}
