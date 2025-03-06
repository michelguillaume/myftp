/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_list.c
*/

#include "myftp.h"
#include "cvector.h"

static void send_message(peer_t *conn, const char *msg, int len)
{
    if (vector_push_back(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

static int verify_auth(peer_t *conn)
{
    if (conn->user_data.state != AUTH) {
        send_message(conn, "530 Not logged in.\r\n", 20);
        return FAILURE;
    }
    return SUCCESS;
}

static int check_access(const char *full_path)
{
    if (access(full_path, R_OK) != 0) {
        return FAILURE;
    }
    return SUCCESS;
}

static void exec_ls_listing(const char *full_dir, peer_t *conn)
{
    int data_sock;

    if (conn->mode == MODE_PASSIVE) {
        data_sock = accept(conn->data_socket, nullptr, nullptr);
        if (data_sock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    } else {
        data_sock = conn->data_socket;
    }
    if (dup2(data_sock, STDOUT_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(data_sock);
    execlp("ls", "ls", "-l", full_dir, (char *)NULL);
    perror("execlp");
    exit(EXIT_FAILURE);
}

// send_message(conn, "451 Requested action aborted. Local error in processing.\r\n", 58);
static void run_listing(const char *full_dir, peer_t *conn)
{
    const pid_t pid = fork();

    if (pid < 0) {
        send_message(conn, "451 Requested action aborted.\r\n", 31);
        close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
        return;
    }
    if (pid == 0) {
        exec_ls_listing(full_dir, conn);
    }
    close(conn->data_socket);
    conn->data_socket = INVALID_SOCKET;
}

static int process_list(server_t *srv, char *arg, peer_t *conn)
{
    char new_pwd[PATH_MAX];
    char resolved[PATH_MAX] = {0};
    const size_t root_len = strlen(srv->path);

    if (arg && arg[0] != '\0')
        if (arg[0] == '/')
            snprintf(new_pwd, PATH_MAX, "%s%s", srv->path, arg);
        else
            snprintf(new_pwd, PATH_MAX,
                "%s%s/%s", srv->path, conn->user_data.pwd, arg);
    else
        snprintf(new_pwd, PATH_MAX, "%s%s", srv->path, conn->user_data.pwd);
    if (realpath(new_pwd, resolved) == NULL)
        return FAILURE;
    if (strncmp(resolved, srv->path, root_len) != 0)
        return FAILURE;
    if (check_access(resolved) == FAILURE)
        return FAILURE;
    run_listing(resolved, conn);
    return SUCCESS;
}

void my_list(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    send_message(conn,
        "150 File status okay; about to open data connection.\r\n", 54);
    if (conn->data_socket == INVALID_SOCKET) {
        send_message(conn, "425 Can't open data connection.\r\n", 33);
        return;
    }
    if (process_list(srv, arg, conn) == FAILURE) {
        send_message(conn, "450 Requested file action not taken.\r\n", 38);
        return;
    }
    send_message(conn, "226 Closing data connection.\r\n", 30);
}
