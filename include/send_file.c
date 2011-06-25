#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUFFER 80

/**
Funkcja przesyla do klienta wskazany przez niego plik.
ETAPY:
1. Przeszukuje baze plikow sesji polaczonych obecnie klientow.
2. Jezeli serwer znajdzie plik w bazie plikow sesji podłączonych użytkowników, przesyła do klienta dane adresowe klienta, który szukany plik posiada.
3. Jeżeli plik nie zostanie znaleziony, funkcja zwraca -1.
4. Po poprawnym ukończeniu przesyłania danych, funkcja zwraca 0.
*/
int send_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
{
    int textDl;
    char textbuffer[MAX_BUFFER+1];
    FILE *fh;
    int whoHasFile;

    printf("INFO: Klient chce otrzymac plik: %s\n",filename);
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */

    if( (whoHasFile = find_file(clientFd, clientaddr, filename)) < 0 )
        return -1;

    /* TODO:
        Trzeba zlapac dane adresowe klienta o ID whoHasFILE - CLIENT_whoHasFile
        Nastepnie przeslac dane adresowe tego goscia co ten plik posiada
    */

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
