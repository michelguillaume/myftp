/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** server_utils.c
*/

#include "myftp.h"
#include "cvector.h"
#include "net_utils.h"

static void handle_received_packet(server_t *srv, peer_t *conn)
{
    char *newline = strstr(conn->receiving_buffer, "\r\n");

    while (newline != NULL) {
        ((short *)newline)[0] = 0;
        process_command(srv, conn);
        vector_erase(conn->receiving_buffer, 0,
            newline + 2 - conn->receiving_buffer);
        newline = strstr(conn->receiving_buffer, "\r\n");
    }
}

int receive_from_peer(server_t *srv, peer_t *conn)
{
    char buffer[1024];
    const ssize_t bytes_received =
        receive_data(conn->socket, buffer, sizeof(buffer));

    if (bytes_received <= 0)
        return FAILURE;
    if (VECTOR_PUSH_BACK(conn->receiving_buffer, buffer, bytes_received)
        == VECTOR_FAILURE) {
        fprintf(stderr, "Error: Failed to push received data to buffer\n");
        return FAILURE;
        }
    handle_received_packet(srv, conn);
    return SUCCESS;
}

int send_to_peer(peer_t *conn)
{
    const ssize_t bytes_forward = send_data(conn->socket, conn->sending_buffer,
        VECTOR_SIZE(conn->sending_buffer));

    if (bytes_forward == -1) {
        printf("Error sending data\n");
        return FAILURE;
    }
    if (bytes_forward != VECTOR_SIZE(conn->sending_buffer)) {
        vector_erase(conn->sending_buffer, 0, bytes_forward);
    } else {
        VECTOR_SIZE(conn->sending_buffer) = 0;
    }
    return SUCCESS;
}
