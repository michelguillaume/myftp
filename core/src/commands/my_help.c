/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_help.c
*/

#include "myftp.h"
#include "cvector.h"

void my_help(server_t *, char *, peer_t *conn)
{
    if (vector_push_back(conn->sending_buffer,
    "214 : USER <SP> <username> <CRLF>   : Specify user for authentication\n"
    "PASS <SP> <password> <CRLF>   : Specify password for authentication\n"
    "CWD  <SP> <pathname> <CRLF>   : Change working directory\n"
    "CDUP <CRLF> : Change working directory to parent directory\n"
    "QUIT <CRLF> : Disconnection\n"
    "DELE <SP> <pathname> <CRLF>   : Delete file on the server\n"
    "PWD  <CRLF>                   : Print working directory\n"
    "PASV <CRLF>                   : Enable passive mode for data transfer\n"
    "PORT <SP> <host-port> <CRLF>  : Enable active mode for data transfer\n"
    "HELP [<SP> <string>] <CRLF>   : List available commands\n"
    "NOOP <CRLF>                   : Do nothing\n"
    "RETR <SP> <pathname> <CRLF>   : Download file from server to client\n"
    "STOR <SP> <pathname> <CRLF>   : Upload file from client to server\n"
    "LIST [<SP> <pathname>] <CRLF> : List files in the current working "
    "directory\r\n", 851) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}
