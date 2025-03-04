#include <stdio.h>
#include "cvector.h"
#include "myftp.h"
#include "net_utils.h"

static void help(void)
{
    (void)puts("USAGE: ./myftp port path\n"
        "       port is the port number on which the server socket listens\n"
        "       path is the path to the home directory for the Anonymous "
        "user\n");
}

server_t *
get_server(void)
{
    static server_t *my_server = nullptr;

    if (!my_server) {
        my_server = calloc(1, sizeof(server_t));
        if (!my_server) {
            return nullptr;
        }
    }
    return my_server;
}

static void signal_handler(const int signum)
{
    server_t *my_server = get_server();

    free(my_server);
    exit(signum);
}

static int get_port(const char *input, uint16_t *port)
{
    char *next;
    const long val = strtol(input, &next, 10);

    if ((next == input) || (*next != '\0') || val < 0 || val > 65535) {
        fprintf(stderr, "Invalid input: %s\n", input);
        return FAILURE;
    }
    *port = (uint16_t)val;
    return SUCCESS;
}

static int get_path(const char *input, char *path)
{
    path = realpath(input, path);
    if (path == NULL) {
        return (FAILURE);
    }
    printf("%s\n", path);
    return SUCCESS;
}

void shutdown_properly(server_t *my_server)
{
    close_connection(my_server->listen_sock);
}

static void
setup_server(server_t *my_server, socket_t listen_sock, uint16_t port, char *path)
{
    my_server->listen_sock = listen_sock;
    my_server->port = port;
    my_server->path = path;
    my_server->connection_list = VECTOR(peer_t, 1024);
    if (my_server->connection_list == nullptr)
        shutdown_properly(my_server);
    my_server->pfds = VECTOR(struct pollfd, 1024);
    if (my_server->pfds == nullptr)
        shutdown_properly(my_server);
    my_server->pfds[0].fd = listen_sock;
    my_server->pfds[0].events = POLLIN;
    vector_size(my_server->pfds) = 1;

//    init_server_data(&my_server->server_data);
}

static void
start_server(const char * const *args)
{
    uint16_t port;
    socket_t listen_sock;
    server_t *my_server;
    char *path = nullptr;

    if (get_port(args[0], &port) == FAILURE || get_path(args[1], path) == FAILURE)
        return;
    listen_sock = create_tcp_server(port);
    if (listen_sock == INVALID_SOCKET)
        return;
    signal(SIGINT, signal_handler);
    my_server = get_server();
    if (!my_server) {
        close_connection(listen_sock);
        return;
    }
    setup_server(my_server, listen_sock, port, path);
    server_loop(my_server);
}

int main(const int ac, const char *const av[])
{
    if (ac == 2 && strcmp(av[1], "-help") == 0) {
        help();
    } else if (ac == 3) {
        start_server(&(av[1]));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
