/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** Handle the PORT command: connect to the client-specified data socket.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "cvector.h"
#include "myftp.h"

static void send_err(peer_t *conn, const char *msg, int len)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer, msg, len) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

static int verify_auth(peer_t *conn)
{
    if (conn->user_data.state != AUTH) {
        send_err(conn, "530 Not logged in.\r\n", 20);
        return FAILURE;
    }
    return SUCCESS;
}

static int verify_args(const char *arg, peer_t *conn)
{
    if (!arg || arg[0] == '\0') {
        send_err(conn,
            "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    return SUCCESS;
}

static int parse_port_arg(const char *arg, char *client_ip, int *client_port)
{
    int a1;
    int a2;
    int a3;
    int a4;
    int p1;
    int p2;

    if (sscanf(arg, "%d,%d,%d,%d,%d,%d",
        &a1, &a2, &a3, &a4, &p1, &p2) != 6)
        return FAILURE;
    if (a1 < 0 || a1 > 255 || a2 < 0 || a2 > 255 ||
        a3 < 0 || a3 > 255 || a4 < 0 || a4 > 255 ||
        p1 < 0 || p1 > 255 || p2 < 0 || p2 > 255)
        return FAILURE;
    snprintf(client_ip, 32, "%d.%d.%d.%d", a1, a2, a3, a4);
    *client_port = p1 * 256 + p2;
    return SUCCESS;
}

static int setup_port_data_socket(const char *client_ip, int client_port)
{
    struct sockaddr_in addr = {0};
    const int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket");
        return SOCKET_ERROR;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(client_port);
    if (inet_pton(AF_INET, client_ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return SOCKET_ERROR;
    }
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return SOCKET_ERROR;
    }
    return sock;
}

static int setup_port_connection(char *arg, peer_t *conn)
{
    char client_ip[32];
    int client_port;
    int data_sock;

    if (parse_port_arg(arg, client_ip, &client_port) == FAILURE) {
        send_err(conn, "501 Syntax error in parameters or arguments.\r\n", 46);
        return FAILURE;
    }
    if (conn->data_socket != INVALID_SOCKET) {
        close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
    }
    data_sock = setup_port_data_socket(client_ip, client_port);
    if (data_sock == SOCKET_ERROR) {
        send_err(conn, "421 Service not available.\r\n", 28);
        return FAILURE;
    }
    conn->data_socket = data_sock;
    conn->mode = MODE_ACTIVE;
    return SUCCESS;
}

//PORT 127,0,0,1,4,210
//4×256 + 210 = 1234
//nc localhost 1234
void my_port(server_t *, char *arg, peer_t *conn)
{
    if (verify_auth(conn) == FAILURE)
        return;
    if (verify_args(arg, conn) == FAILURE)
        return;
    if (setup_port_connection(arg, conn) == FAILURE)
        return;
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "200 Command okay.\r\n", 19) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push PORT response\n");
}
