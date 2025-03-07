/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** server.c
*/

#include "cvector.h"
#include "myftp.h"
#include "net_utils.h"

static void update_pfds(peer_t* connection_list, struct pollfd* pfds)
{
    for (int i = 1; i <= VECTOR_SIZE(connection_list); i++) {
        if (VECTOR_SIZE(connection_list[i - 1].sending_buffer) != 0) {
            pfds[i].events |= POLLOUT;
        } else {
            pfds[i].events &= ~POLLOUT;
        }
    }
}

static int init_peer_connection(peer_t *conn,
    const socket_t client_sock, const struct sockaddr_in client_addr)
{
    conn->sending_buffer = VECTOR(char, 1024);
    if (conn->sending_buffer == NULL)
        return FAILURE;
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "220 Service ready for new user.\r\n", 33) == VECTOR_FAILURE) {
        VECTOR_DESTROY(conn->receiving_buffer);
        return FAILURE;
    }
    conn->receiving_buffer = VECTOR(char, 1024);
    if (conn->receiving_buffer == NULL) {
        VECTOR_DESTROY(conn->sending_buffer);
        return FAILURE;
    }
    conn->socket = client_sock;
    conn->address = client_addr;
    conn->mode = MODE_NONE;
    conn->data_socket = INVALID_SOCKET;
    conn->should_quit = false;
    conn->user_data = (user_data_t){NOT_AUTH, {0}, {0}};
    return SUCCESS;
}

static void handle_accepted_connection(server_t* srv,
    const socket_t client_sock, const struct sockaddr_in client_addr)
{
    peer_t conn = {0};

    if (init_peer_connection(&conn, client_sock, client_addr) == FAILURE) {
        close_connection(client_sock);
        return;
    }
    if (VECTOR_PUSH_BACK(srv->connection_list, (peer_t[]){conn}, 1)
        == VECTOR_FAILURE) {
        close_connection(client_sock);
        VECTOR_DESTROY(conn.receiving_buffer);
        VECTOR_DESTROY(conn.sending_buffer);
        return;
    }
    if (VECTOR_PUSH_BACK(srv->pfds,
        ((struct pollfd[]){{.fd = client_sock,
        .events = POLLIN, .revents = 0}}), 1) == VECTOR_FAILURE) {
        close_connection(client_sock);
        VECTOR_DESTROY(conn.receiving_buffer);
        VECTOR_DESTROY(conn.sending_buffer);
    }
}

static void handle_new_connection(server_t *srv)
{
    struct sockaddr_in client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    const socket_t client_sock = accept_tcp_connection(srv->listen_sock,
        (struct sockaddr*)&client_addr, &addr_len);

    if (client_sock == INVALID_SOCKET)
        return;
    handle_accepted_connection(srv, client_sock, client_addr);
}

static void handle_listen_socket_events(server_t *srv)
{
    if (srv->pfds[0].revents & POLLERR) {
        fprintf(stderr, "Error: Listen socket encountered POLLERR\n");
        return;
    }
    if (srv->pfds[0].revents & POLLIN) {
        handle_new_connection(srv);
    }
}

static void close_client_connection(server_t *srv, peer_t *conn, int idx)
{
    abort_connection(conn->socket);
    VECTOR_DESTROY(conn->receiving_buffer);
    VECTOR_DESTROY(conn->sending_buffer);
    if (idx != VECTOR_SIZE(srv->connection_list)) {
        memmove(srv->connection_list + idx - 1,
            VECTOR_BACK(srv->connection_list), sizeof(peer_t));
        memmove(srv->pfds + idx,
            VECTOR_BACK(srv->pfds), sizeof(struct pollfd));
    }
    VECTOR_HEADER(srv->connection_list)->size--;
    VECTOR_HEADER(srv->pfds)->size--;
}

static void process_socket(server_t *srv, int idx)
{
    peer_t* conn = srv->connection_list + idx - 1;

    if (srv->pfds[idx].revents & POLLERR) {
        printf("Error: Socket encountered POLLERR\n");
        close_client_connection(srv, conn, idx);
        return;
    }
    if (srv->pfds[idx].revents & POLLIN
        && receive_from_peer(srv, conn) == FAILURE) {
        close_client_connection(srv, conn, idx);
        return;
    }
    if (srv->pfds[idx].revents & POLLOUT && send_to_peer(conn) == FAILURE) {
        close_client_connection(srv, conn, idx);
    }
    if (conn->should_quit == true
        && VECTOR_SIZE(conn->sending_buffer) == 0) {
        close_client_connection(srv, conn, idx);
    }
}

static void process_client_connections(server_t *srv)
{
    for (int i = VECTOR_SIZE(srv->connection_list); i > 0; i--) {
        process_socket(srv, i);
    }
}

_Noreturn void server_loop(server_t *srv)
{
    int ready;

    while (1) {
        update_pfds(srv->connection_list, srv->pfds);
        ready = poll(srv->pfds, VECTOR_SIZE(srv->pfds), -1);
        if (ready == -1) {
            fprintf(stderr, "Error: poll failed: %s\n", strerror(errno));
            continue;
        }
        process_client_connections(srv);
        handle_listen_socket_events(srv);
    }
}
