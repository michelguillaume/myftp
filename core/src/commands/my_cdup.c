/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_cdup.c
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

static int verify_args(const char *arg, peer_t *conn)
{
    if (arg && arg[0] != '\0') {
        send_error(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static void update_pwd_to_parent(char *pwd)
{
    char *last_slash = strrchr(pwd, '/');

    if (last_slash != NULL) {
        if (last_slash == pwd)
            pwd[1] = '\0';
        else
            *last_slash = '\0';
    }
}

static void send_success(peer_t *conn)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "250 Requested file action okay, completed.\r\n", 44)
        == VECTOR_FAILURE)
        fprintf(stderr,
            "Error: Failed to push success message to sending_buffer\n");
}

void my_cdup(server_t *, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    if (strcmp(conn->user_data.pwd, "/") == 0) {
        send_success(conn);
        return;
    }
    update_pwd_to_parent(conn->user_data.pwd);
    send_success(conn);
}
