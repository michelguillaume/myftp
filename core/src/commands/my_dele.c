/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_dele.c
*/

#include "myftp.h"
#include "cvector.h"
#include <sys/stat.h>

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
    if (!arg || arg[0] == '\0') {
        send_error(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static int perform_file_deletion(server_t *srv, peer_t *conn, char *arg)
{
    char full_path[PATH_MAX];
    char resolved[PATH_MAX];
    const size_t root_len = strlen(srv->path);

    if (arg[0] == '/') {
        snprintf(full_path, sizeof(full_path), "%s%s", srv->path, arg);
    } else {
        snprintf(full_path, sizeof(full_path),
            "%s%s/%s", srv->path, conn->user_data.pwd, arg);
    }
    if (realpath(full_path, resolved) == NULL)
        return FAILURE;
    if (strncmp(resolved, srv->path, root_len) != 0)
        return FAILURE;
    if (remove(resolved) != 0)
        return FAILURE;
    return SUCCESS;
}

static void send_success(peer_t *conn)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "250 Requested file action okay, completed.\r\n", 44)
        == VECTOR_FAILURE)
        fprintf(stderr,
            "Error: Failed to push success message to sending_buffer\n");
}

void my_dele(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    if (perform_file_deletion(srv, conn, arg) == FAILURE) {
        send_error(conn, "550 Requested action not taken.\r\n", 33);
        return;
    }
    send_success(conn);
}
