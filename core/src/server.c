#include <math.h>
#include <net_utils.h>

#include "myftp.h"
#include "cvector.h"

void update_pfds(peer_t *connection_list, struct pollfd *pfds)
{
    if (vector_reserve(pfds, vector_size(connection_list)) == VECTOR_FAILURE)
#warning return or call return_properly
        return;
    for (int i = 1; i <= vector_size(connection_list); i++) {
        if (connection_list[i - 1].socket == INVALID_SOCKET)
            continue;
        pfds[i].fd = connection_list[i - 1].socket;
        if (vector_size(connection_list[i - 1].sending_buffer) == 0) {
            pfds[i].events = POLLIN;
        } else {
            pfds[i].events = POLLIN | POLLOUT;
        }
    }
}

void handle_accepted_connection(server_t *my_server, socket_t client_sock, struct sockaddr_in client_addr)
{
    peer_t new_connection = {0};

    new_connection.receiving_buffer = VECTOR(char, 1024);
    if (new_connection.receiving_buffer == NULL)
#warning return or call return_properly
        return;
    new_connection.sending_buffer = VECTOR(char, 1024);
    if (new_connection.sending_buffer == NULL)
#warning return or call return_properly
        return;
    new_connection.socket = client_sock;
    new_connection.address = client_addr;
    if (vector_push_back(my_server->connection_list, (peer_t []){new_connection}, 1) == VECTOR_FAILURE)
#warning return or call return_properly
        return;
}

void handle_new_connection(server_t * my_server)
{
    struct sockaddr_in client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    socket_t client_sock = accept_tcp_connection(my_server->listen_sock,
        (struct sockaddr*)&client_addr, &addr_len);

    if (client_sock == INVALID_SOCKET)
#warning return or call return_properly
        return;
    handle_accepted_connection(my_server, client_sock, client_addr);
}

void handle_listen_socket_events(server_t *my_server)
{
    if (my_server->pfds[0].events & POLLERR) {
        fprintf(stdout, "listen socket failed\n");
#warning return or call return_properly
        return;
    }
    if (my_server->pfds[0].events & POLLIN) {
        handle_new_connection(my_server);
    }
}

void close_client_connection(server_t *my_server, peer_t *connection, int idx)
{
    close_connection(connection->socket);
    vector_destroy(connection->receiving_buffer);
    vector_destroy(connection->sending_buffer);
    if (idx == vector_size(my_server->connection_list) - 1) {
        return;
    }
    memmove(my_server->connection_list + idx,
        vector_back(my_server->connection_list), sizeof(peer_t));
}

void handle_received_packet(server_t *my_server, peer_t *client)
{
    char *newline = strstr(client->receiving_buffer, "\r\n");

    while (newline != NULL) {
        ((short *)newline)[0] = 0;
        process_command(my_server, client);
        vector_erase(client->receiving_buffer, 0, (size_t)(newline + 2 - client->receiving_buffer));
        newline = strstr(client->sending_buffer, "\r\n");
    }
}

int receive_from_peer(server_t *my_server, peer_t *client)
{
    char buffer[1024];
    ssize_t bytes_received = receive_data(client->socket, buffer, sizeof(buffer));

    if (bytes_received < 0)
        return FAILURE;
    if (vector_push_back(client->receiving_buffer, buffer, bytes_received) == VECTOR_FAILURE)
    {
#warning return or call return_properly

    }
    handle_received_packet(my_server, client);
    return SUCCESS;
}

void process_socket(server_t *my_server, int idx)
{
    peer_t *client = my_server->connection_list + idx;

    if (my_server->pfds[idx].revents & POLLERR) {
        close_client_connection(my_server, client, idx);
        return;
    }
    if (my_server->pfds[idx].revents & POLLIN &&
        receive_from_peer(my_server, client) == FAILURE) {
        close_client_connection(my_server, client, idx);
        return;
        }

}

void process_client_connections(server_t *my_server)
{
    for (int i = vector_size(my_server->connection_list) - 1; i > 0 ; i--) {
        process_socket(my_server, i);
    }
}

_Noreturn void server_loop(server_t *my_server)
{
    /*
    fd_set_group_t fds_group = { };
    int high_sock;

    do {
        update_fd_sets(my_server, &high_sock, &fds_group);
        if (select(high_sock + 1, &fds_group.read_fds,
            &fds_group.write_fds, &fds_group.except_fds, NULL) <= 0) {
            (void)printf("select: %s", strerror(errno));
            shutdown_properly(my_server, EXIT_FAILURE);
            }
        handle_listen_socket_events(my_server, &fds_group);
        process_client_connections(my_server, &fds_group);
    } while (1);
    */
    int ready;

    while (1) {
        update_pfds(my_server->connection_list, my_server->pfds);
        ready = poll(my_server->pfds, vector_size(my_server->pfds), -1);
        if (ready == -1) {
            fprintf(stdout, "%s: %s\n", "poll failed", strerror(errno));
            continue;
        }
        process_client_connections(my_server);
        handle_listen_socket_events(my_server);
    }
}
