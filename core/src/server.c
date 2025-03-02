#include "myftp.h"
#include "cvector.h"

void update_pfds(server_t *my_server)
{
    for (int i = 1; i <= vector_size(my_server->connection_list); i++) {
        if (my_server->connection_list[i - 1].socket == INVALID_SOCKET)
            continue;
        my_server->pfds[i].fd = my_server->connection_list[i - 1].socket;
        if (vector_size(my_server->connection_list[i - 1].sending_buffer) == 0)
            my_server->pfds[i].events = POLLIN;
        else
            my_server->pfds[i].events = POLLIN | POLLOUT;
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

    while (1) {
        update_pfds(my_server);

    }
}
