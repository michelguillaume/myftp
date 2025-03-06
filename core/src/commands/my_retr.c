/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_retr.c
*/

#include "myftp.h"
#include "cvector.h"
#include <sys/stat.h>
#include <fcntl.h>

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

static int verify_args(char *arg, peer_t *conn)
{
    if (!arg || arg[0] == '\0') {
        send_message(conn, "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static int check_access(const char *full_path)
{
    struct stat st;

    if (access(full_path, R_OK) != 0) {
        return FAILURE;
    }
    if (stat(full_path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return FAILURE;
    }
    return SUCCESS;
}

static int copy_data(int file_fd, int data_sock)
{
    ssize_t bytes_read;
    ssize_t bytes_written;
    char buffer[4096];

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(data_sock, buffer, bytes_read);
        if (bytes_written < 0) {
            return -1;
        }
    }
    if (bytes_read < 0)
        return -1;
    return 0;
}

static void transfer_file(const char *full_path, int data_sock, peer_t *conn)
{
    const int file_fd = open(full_path, O_RDONLY);

    if (file_fd < 0) {
        perror("open file");
        write(conn->socket, "550 Requested action not taken.\r\n", 33);
        close(data_sock);
        exit(EXIT_FAILURE);
    }
    if (copy_data(file_fd, data_sock) < 0) {
        perror("read/write error");
        write(conn->socket, "426 Connection closed; transfer aborted.\r\n", 42);
        close(file_fd);
        close(data_sock);
        exit(EXIT_FAILURE);
    }
    close(file_fd);
    close(data_sock);
    write(conn->socket, "226 Transfer complete.\r\n", 24);
    exit(EXIT_SUCCESS);
}

static void exec_retr(const char *full_path, peer_t *conn)
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

/*
    if (dup2(data_sock, STDOUT_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(data_sock);
    execlp("cat", "cat", full_path, (char *)NULL);
    perror("execlp");
    exit(EXIT_FAILURE);
*/
}

static void run_retr(const char *full_dir, peer_t *conn)
{
    const pid_t pid = fork();

    if (pid < 0) {
        send_message(conn, "451 Requested action aborted.\r\n", 31);
        close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
        return;
    }
    if (pid == 0) {
        exec_retr(full_dir, conn);
    }
    close(conn->data_socket);
    conn->data_socket = INVALID_SOCKET;
}

static int process_retr(server_t *srv, char *arg, peer_t *conn)
{
    char new_pwd[PATH_MAX];
    char resolved[PATH_MAX] = {0};
    const size_t root_len = strlen(srv->path);

    if (arg[0] == '/') {
        snprintf(new_pwd, PATH_MAX, "%s%s", srv->path, arg);
    } else {
        snprintf(new_pwd, PATH_MAX, "%s%s/%s",
            srv->path, conn->user_data.pwd, arg);
    }
    if (realpath(new_pwd, resolved) == NULL)
        return FAILURE;
    if (strncmp(resolved, srv->path, root_len) != 0)
        return FAILURE;
    if (check_access(resolved) == FAILURE)
        return FAILURE;
    run_retr(resolved, conn);
    return SUCCESS;
}

void my_retr(server_t *srv, char *arg, peer_t *conn)
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
    if (process_retr(srv, arg, conn) == FAILURE) {
        send_message(conn, "550 Requested action not taken.\r\n", 33);
    }
}
