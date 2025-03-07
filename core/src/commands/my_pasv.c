/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_pasv.c
*/

#include <net_utils.h>


#include "cvector.h"
#include "myftp.h"

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

/*
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_port = 0;
*/
static int create_pasv_socket(void)
{
    struct sockaddr_in addr = {0};
    const int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket");
        return SOCKET_ERROR;
    }
    addr.sin_family = AF_INET;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return SOCKET_ERROR;
    }
    if (listen(sock, 1) < 0) {
        perror("listen");
        close(sock);
        return SOCKET_ERROR;
    }
    return sock;
}

static int get_pasv_port(const int sock)
{
    struct sockaddr_in addr = {0};
    socklen_t addr_len = sizeof(addr);

    if (getsockname(sock, (struct sockaddr *)&addr, &addr_len) < 0) {
        perror("getsockname");
        return SOCKET_ERROR;
    }
    return (ntohs(addr.sin_port));
}

static void send_pasv_response(server_t *srv, peer_t *conn, int port)
{
    char response[128];
    int h1;
    int h2;
    int h3;
    int h4;
    const int p1 = port / 256;
    const int p2 = port % 256;

    sscanf(srv->ip, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
    snprintf(response, sizeof(response),
        "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n",
        h1, h2, h3, h4, p1, p2);
    if (VECTOR_PUSH_BACK(conn->sending_buffer, response, strlen(response))
        == VECTOR_FAILURE) {
        fprintf(stderr, "Error: Failed to push PASV response\n");
    }
}

static void setup_pasv_data_socket(server_t *srv, peer_t *conn)
{
    int port;
    int data_sock = create_pasv_socket();

    if (data_sock < 0) {
        send_error(conn, "421 Service not available.\r\n", 30);
        return;
    }
    port = get_pasv_port(data_sock);
    if (port < 0) {
        close(data_sock);
        send_error(conn, "501 Service not available.\r\n", 30);
        return;
    }
    conn->data_socket = data_sock;
    conn->mode = MODE_PASSIVE;
    send_pasv_response(srv, conn, port);
}

void my_pasv(server_t *srv, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    if (conn->data_socket != INVALID_SOCKET) {
        close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
    }
    setup_pasv_data_socket(srv, conn);
}
