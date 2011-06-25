/**
Plik nagłówkowy
*/

#ifndef KLIENT_FUNKCJE_H
#define KLIENT_FUNKCJE_H

#define MAX_BUFFER              128
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
int send_message(int clientFd, struct sockaddr_in clientaddr, char textbuffer[MAX_BUFFER+1]);
int send_file(int clientFd, struct sockaddr_in clientaddr, char filename[32]);
int find_file(int clientFd, struct sockaddr_in clientaddr, char filename[32]);
void save_file_list(char buff[255], char clientNo[16]);
#endif
