#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUFFER 80
#define MAXCLIENTS 2
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
    //char textbuffer[MAX_BUFFER+1];
    printf("INFO: Szukam pliku: %s\n", filename);
    char word[MAX_BUFFER];
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */
    int i=0;

    while(i<MAXCLIENTS)
    {
        sprintf(filepath, "session/CLIENT_%i", i);
        if ((fh = fopen(filepath, "r")) != NULL)
        {
            printf("Szukam w pliku: %s\n", filepath);
            while (fscanf(fh, "%s", word) == 1)
            {
                if(strncmp(word, filename, strlen(filename)) == 0 )
                {
                    fclose(fh);
                    return 0;
                }
            }
            fclose(fh);
            i++;
            continue;
        }
        /* jezeli pliku nie ma, zwieksz licznik */
        i++;
    }
    return -1;
}
