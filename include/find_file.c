#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUFFER 80
/**
Przesyla do klienta zawartosc pliku o zadanej nazwie.
Jako parametry pobiera deskryptor gniazda i strukture adresowa klienta oraz nazwe pliku.
Jesli plik o zadanej nazwie istnieje i serwer moze z nigo czytac, przesyla w petli jego zawartosc do klienta paczkami o rozmiarze MAX_BUFFER.
W serwerze wykorzystywana do przeslania do klienta danych adresowych innego klienta, udostepniajacego dany plik. Dane te sa zapisane w pliku sesji.
**/

int find_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
{
    FILE *fh;
    char filepath[MAX_BUFFER];
    printf("INFO: Szukam pliku: %s.\n", filename);
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */
    sprintf(filepath, "files/%s", filename);
    if ((fh = fopen(filepath, "r")) != NULL)
    {
        /* Jezeli plik istnieje ! */
        return 0;
    }
    /* jezeli plik nie istnieje */
    else
        return -1;
}
