/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_stor.c
*/

#include "myftp.h"
#include "cvector.h"

static void send_message(peer_t *conn, const char *msg, int len)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
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

static int verify_args(char *arg, peer_t *conn)
{
    if (!arg || arg[0] == '\0') {
        send_message(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static int check_directory(const char *full_path, server_t *srv)
{
    char dir[PATH_MAX];
    char resolved_dir[PATH_MAX];
    char *dirname_ptr;
    const size_t root_len = strlen(srv->path);

    strncpy(dir, full_path, sizeof(dir));
    dir[sizeof(dir) - 1] = '\0';
    dirname_ptr = dirname(dir);
    if (realpath(dirname_ptr, resolved_dir) == NULL)
        return FAILURE;
    if (strncmp(resolved_dir, srv->path, root_len) != 0)
        return FAILURE;
    if (access(dirname_ptr, W_OK) != 0)
        return FAILURE;
    return SUCCESS;
}

static int copy_data(int file_fd, int data_sock)
{
    ssize_t bytes_written;
    char buffer[4096];
    ssize_t bytes_read = read(data_sock, buffer, sizeof(buffer));

    while (bytes_read > 0) {
        bytes_written = write(file_fd, buffer, bytes_read);
        if (bytes_written < 0) {
            return -1;
        }
        bytes_read = read(data_sock, buffer, sizeof(buffer));
    }
    if (bytes_read < 0)
        return -1;
    return 0;
}

static void transfer_file(const char *full_path, int data_sock, peer_t *conn)
{
    const int file_fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (file_fd < 0) {
        perror("open file");
        write(conn->socket, "550 Requested action not taken.\r\n", 33);
        close(data_sock);
        exit(EXIT_FAILURE);
    }
    if (copy_data(file_fd, data_sock) < 0) {
        perror("read/write error");
        write(conn->socket,
            "426 Connection closed; transfer aborted.\r\n", 42);
        close(file_fd);
        close(data_sock);
        exit(EXIT_FAILURE);
    }
    close(file_fd);
    close(data_sock);
    write(conn->socket, "226 Transfer complete.\r\n", 24);
    exit(EXIT_SUCCESS);
}

static void exec_stor(const char *full_path, peer_t *conn)
{
    int data_sock;

    if (conn->mode == MODE_PASSIVE) {
        data_sock = accept(conn->data_socket, nullptr, nullptr);
        if (data_sock < 0) {
            perror("accept");
            write(conn->socket, "425 Can't open data connection.\r\n", 33);
            exit(EXIT_FAILURE);
        }
    } else {
        data_sock = conn->data_socket;
    }
    transfer_file(full_path, data_sock, conn);
}

static void run_stor(const char *full_path, peer_t *conn)
{
    const pid_t pid = fork();

    if (pid < 0) {
        send_message(conn, "451 Requested action aborted.\r\n", 31);
        close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
        return;
    }
    if (pid == 0) {
        exec_stor(full_path, conn);
    }
    close(conn->data_socket);
    conn->data_socket = INVALID_SOCKET;
}

static int process_stor(server_t *srv, char *arg, peer_t *conn)
{
    char new_pwd[PATH_MAX];

    if (arg[0] == '/')
    {
        snprintf(new_pwd, PATH_MAX, "%s%s", srv->path, arg);
    } else {
        snprintf(new_pwd, PATH_MAX, "%s%s/%s",
            srv->path, conn->user_data.pwd, arg);
    }
    if (check_directory(new_pwd, srv) == FAILURE)
        return FAILURE;
    run_stor(new_pwd, conn);
    return SUCCESS;
}

// telnet 127.0.0.1 57811 < Makefile
void my_stor(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    send_message(conn,
        "150 File status okay; about to open data connection.\r\n", 54);
    if (conn->data_socket == INVALID_SOCKET) {
        send_message(conn, "425 Can't open data connection.\r\n", 33);
        return;
    }
    if (process_stor(srv, arg, conn) == FAILURE) {
        send_message(conn, "550 Requested action not taken.\r\n", 33);
    }
}
