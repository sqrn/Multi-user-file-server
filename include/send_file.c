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

int send_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
{
    int textDl;
    char textbuffer[MAX_BUFFER+1];
    FILE *fh;
    printf("Szukam pliku %s",filename);
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */
    sprintf(filename, "files/%s", filename);
    if ((fh = fopen(filename, "r")) != NULL)
    {
        while (!feof(fh))
        {
            fread(textbuffer, 1, MAX_BUFFER, fh);
            textDl = strlen(textbuffer);
            if (send(clientFd, textbuffer, textDl, 0) < 0)
                return -1;
        }
        fclose(fh);
    }
    else
        return -1;

    return 0;
}
