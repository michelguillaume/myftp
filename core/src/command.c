/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
*/

#include "myftp.h"
#include "cvector.h"

static fnct_ptr_t commands[] = {
{"USER", my_user},
    /*

    {"PASS", my_pass},
    {"CWD", my_cwd},
    {"CDUP", my_cdup},
    {"QUIT", my_quit},
    {"DELE", my_dele},
    {"PWD", my_pwd},
    {"PASV", my_pasv},
    {"PORT", my_port},
    {"HELP", my_help},
    {"NOOP", my_noop},
    {"RETR", my_retr},
    {"STOR", my_stor},
    {"LIST", my_list},
     */
};

#ifdef __APPLE__

char *strchrnul(const char *str, int c) {
    const char *ptr = str;

    while (*ptr != '\0' && *ptr != c) {
        ++ptr;
    }
    return (char *)ptr;
}

#endif

void process_command(server_t *srv, peer_t *conn)
{
    char *ptr = strchrnul(conn->receiving_buffer, ' ');

    if (*ptr)
       *ptr = 0;
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strcmp(commands[i].str, conn->receiving_buffer) == 0) {
            printf("Command: %s\n", commands[i].str);
            commands[i].ptr(srv,
                (vector_back(conn->receiving_buffer) == ptr)
                ? nullptr : ptr + 1, conn);
            return;
        }
    }
    if (vector_push_back(conn->sending_buffer, "500\r\n", 5) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
}
