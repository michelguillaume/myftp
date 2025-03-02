/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
*/

#include "myftp.h"
#include "cvector.h"

void my_user(server_t *my_server, char *input, peer_t *client)
{

}

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
    {0}
};


void process_command(server_t *my_server, peer_t *client)
{
    char *ptr = strchrnul(client->receiving_buffer, ' ');

    if (*ptr)
       *ptr = 0;
    for (int i = 0; commands[i].ptr; i++) {
        if (strcmp(commands[i].str, client->receiving_buffer) == 0) {
            commands[i].ptr(my_server, ptr, client);
            return;
        }
    }
    if (vector_push_back(client->sending_buffer, "500\r\n", 5) == VECTOR_FAILURE)
    {
#warning return or call return_properly
    }
}
