/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
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
    if (!arg || arg[0] == '\0') {
        send_error(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static int check_directory(const char *resolved)
{
    struct stat st;

    if (stat(resolved, &st) != 0 || !S_ISDIR(st.st_mode))
        return (FAILURE);
    return (SUCCESS);
}

static int update_virtual_path(peer_t *conn, const char *resolved,
    const size_t root_len)
{
    if (resolved[root_len] == '\0') {
        conn->user_data.pwd[0] = '/';
        conn->user_data.pwd[1] = '\0';
    } else {
        strcpy(conn->user_data.pwd, resolved + root_len);
    }
    return (SUCCESS);
}

static int update_pwd(server_t *srv, peer_t *conn, char *new_dir)
{
    char new_pwd[PATH_MAX];
    char resolved[PATH_MAX] = {0};
    const size_t root_len = strlen(srv->path);

    if (new_dir[0] == '/') {
        snprintf(new_pwd, sizeof(new_pwd), "%s%s", srv->path, new_dir);
    } else {
        snprintf(new_pwd, sizeof(new_pwd),
            "%s%s/%s", srv->path, conn->user_data.pwd, new_dir);
    }
    if (realpath(new_pwd, resolved) == NULL)
        return FAILURE;
    if (strncmp(resolved, srv->path, root_len) != 0)
        return FAILURE;
    if (check_directory(resolved) == FAILURE)
        return FAILURE;
    update_virtual_path(conn, resolved, root_len);
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

void my_cwd(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    if (update_pwd(srv, conn, arg) == FAILURE) {
        send_error(conn, "550 Requested action not taken.\r\n", 33);
        return;
    }
    send_success(conn);
}
