#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUFFER              80

/*!
\file send_message.c
Ogólna funkcja programowa. Przesyła do połączonego klienta komunikat
\retval -1 komunikat nie został przesłany
\retval 0 jeżeli komunikat zostanie przesłany poprawnie
**/
int send_message(int clientFd, struct sockaddr_in clientaddr, char textbuffer[MAX_BUFFER+1])
{
    int textDl;
    printf("INFO: Wysyłam do klienta wiadomosc: %s\n", textbuffer);
    textDl = strlen(textbuffer);
    //if (sendto(serverFd, textbuffer, textDl, 0, (struct sockaddr *) &clientaddr, sizeof(clientaddr)) < 0)
    if(send(clientFd, textbuffer, textDl, 0) < 0 )
        return -1;
    else
        return 0;

}
