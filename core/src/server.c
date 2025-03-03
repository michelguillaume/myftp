#include <math.h>
#include <net_utils.h>

#include "cvector.h"
#include "myftp.h"

void update_pfds(peer_t* connection_list, struct pollfd* pfds)
{
    for (int i = 1; i <= vector_size(connection_list); i++) {
        if (vector_size(connection_list[i - 1].sending_buffer) != 0) {
            pfds[i].events |= POLLOUT;
        } else {
            pfds[i].events &= ~POLLOUT;
        }
    }
}

static int init_peer_connection(peer_t* conn, socket_t client_sock, struct sockaddr_in client_addr)
{
    conn->receiving_buffer = VECTOR(char, 1024);
    if (conn->receiving_buffer == NULL) {
        fprintf(stderr, "Error: Failed to allocate receiving buffer\n");
        return FAILURE;
    }
    conn->sending_buffer = VECTOR(char, 1024);
    if (conn->sending_buffer == NULL) {
        fprintf(stderr, "Error: Failed to allocate sending buffer\n");
        vector_destroy(conn->receiving_buffer);
        return FAILURE;
    }
    conn->socket = client_sock;
    conn->address = client_addr;
    conn->user_data = (user_data_t){NOT_AUTH, {0}, {0}};
    return SUCCESS;
}

void handle_accepted_connection(server_t* srv, socket_t client_sock, struct sockaddr_in client_addr)
{
    peer_t conn = {0};

    if (init_peer_connection(&conn, client_sock, client_addr) == VECTOR_FAILURE) {
        close_connection(client_sock);
        return;
    }
    if (vector_push_back(srv->connection_list, (peer_t[]){conn}, 1) == VECTOR_FAILURE) {
        fprintf(stderr, "Error: Failed to add peer to connection list\n");
        close_connection(client_sock);
        vector_destroy(conn.receiving_buffer);
        vector_destroy(conn.sending_buffer);
        return;
    }
    //if (vector_push_back(srv->pfds, ((struct pollfd[]){{.fd = client_sock, .events = POLLIN | POLLRDHUP, .revents = 0}}), 1) == VECTOR_FAILURE) {
    if (vector_push_back(srv->pfds, ((struct pollfd[]){{.fd = client_sock, .events = POLLIN, .revents = 0}}), 1) == VECTOR_FAILURE) {
        fprintf(stderr, "Error: Failed to add pollfd for new connection\n");
        close_connection(client_sock);
        vector_destroy(conn.receiving_buffer);
        vector_destroy(conn.sending_buffer);
    }
}

void handle_new_connection(server_t* srv)
{
    struct sockaddr_in client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    const socket_t client_sock = accept_tcp_connection(srv->listen_sock, (struct sockaddr*)&client_addr, &addr_len);

    if (client_sock == INVALID_SOCKET)
        return;
    handle_accepted_connection(srv, client_sock, client_addr);
}

void handle_listen_socket_events(server_t* srv)
{
    if (srv->pfds[0].revents & POLLERR) {
        fprintf(stderr, "Error: Listen socket encountered POLLERR\n");
        return;
    }
    if (srv->pfds[0].revents & POLLIN) {
        handle_new_connection(srv);
    }
}

void close_client_connection(server_t* srv, peer_t* conn, int idx)
{
    abort_connection(conn->socket);
    vector_destroy(conn->receiving_buffer);
    vector_destroy(conn->sending_buffer);
    if (idx != vector_size(srv->connection_list)) {
        memmove(srv->connection_list + idx - 1, vector_back(srv->connection_list), sizeof(peer_t));
        memmove(srv->pfds + idx, vector_back(srv->pfds), sizeof(struct pollfd));
    }
    VECTOR_HEADER(srv->connection_list)->size--;
    VECTOR_HEADER(srv->pfds)->size--;
}

void handle_received_packet(server_t* srv, peer_t* conn)
{
    char* newline = strstr(conn->receiving_buffer, "\r\n");

    while (newline != NULL) {
        ((short*)newline)[0] = 0;
        process_command(srv, conn);
        vector_erase(conn->receiving_buffer, 0, newline + 2 - conn->receiving_buffer);
        newline = strstr(conn->receiving_buffer, "\r\n");
    }
}

int receive_from_peer(server_t* srv, peer_t* conn)
{
    char buffer[1024];
    const ssize_t bytes_received = receive_data(conn->socket, buffer, sizeof(buffer));

    if (bytes_received <= 0)
        return FAILURE;
    if (vector_push_back(conn->receiving_buffer, buffer, bytes_received) == VECTOR_FAILURE) {
        fprintf(stderr, "Error: Failed to push received data to buffer\n");
        return FAILURE;
    }
    handle_received_packet(srv, conn);
    return SUCCESS;
}


int send_to_peer(peer_t *conn)
{
    const ssize_t bytes_forward = send_data(conn->socket, conn->sending_buffer,
        vector_size(conn->sending_buffer));

    if (bytes_forward == -1) {
        printf("Error sending data\n");
        return FAILURE;
    }
    if (bytes_forward != vector_size(conn->sending_buffer)) {
        vector_erase(conn->sending_buffer, 0, bytes_forward);
    } else {
        vector_size(conn->sending_buffer) = 0;
    }
    return SUCCESS;
}

void process_socket(server_t* srv, int idx)
{
    peer_t* conn = srv->connection_list + idx - 1;

    if (srv->pfds[idx].revents & POLLERR) {
        printf("Error: Socket encountered POLLERR\n");
        close_client_connection(srv, conn, idx);
        return;
    }
    if (srv->pfds[idx].revents & POLLIN && receive_from_peer(srv, conn) == FAILURE) {
        close_client_connection(srv, conn, idx);
        return;
    }
    if (srv->pfds[idx].revents & POLLOUT && send_to_peer(conn) == FAILURE) {
        close_client_connection(srv, conn, idx);
    }
}

void process_client_connections(server_t* srv)
{
    for (int i = vector_size(srv->connection_list); i > 0; i--) {
        process_socket(srv, i);
    }
}

_Noreturn void server_loop(server_t* srv)
{
    int ready;

    while (1) {
        update_pfds(srv->connection_list, srv->pfds);
        ready = poll(srv->pfds, vector_size(srv->pfds), -1);
        if (ready == -1) {
            fprintf(stderr, "Error: poll failed: %s\n", strerror(errno));
            continue;
        }
        process_client_connections(srv);
        handle_listen_socket_events(srv);
    }
}
