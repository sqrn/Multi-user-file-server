#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include "fun.h"


/**
Funkcja przesyla do klienta wskazany przez niego plik.
ETAPY:
1. Przeszukuje baze plikow sesji polaczonych obecnie klientow.
2. Jezeli serwer znajdzie plik w bazie plikow sesji podłączonych użytkowników, przesyła do klienta dane adresowe klienta, który szukany plik posiada.
3. Jeżeli plik nie zostanie znaleziony, funkcja zwraca -1.
4. Po poprawnym ukończeniu przesyłania danych, funkcja zwraca 0.
*/
int send_file(int clientFd, struct sockaddr_in clientaddr, char filename[32], int cli_id)
{
    char textbuffer[MAX_BUFFER+1];
    int whoHasFile;
    char cli_port[20];
    char cli_addr[20];


    printf("INFO: Klient chce otrzymac plik: %s\n",filename);
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */

    printf("Plik posiada CLIENT_%i", cli_id);

/*    if(getpeername(clientFd, (struct sockaddr *)&clientaddr, &klientDl) != 0)
        perror("ERROR: getpeername()");
    else
    {
        printf("INFO: Doszedl nowy klient: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        sprintf(cli_addr, "%s", inet_ntoa(clientaddr.sin_addr));
        sprintf(cli_port, "%d", ntohs(clientaddr.sin_port));

    }
*/
    /* TODO:
        Trzeba zlapac dane adresowe klienta o ID whoHasFILE - CLIENT_whoHasFile
        Nastepnie przeslac dane adresowe tego goscia co ten plik posiada
    */
    /* przesyla do klienta dane adresowe klienta, ktory posiada plik */
    /*
    przeslac krotke do klienta:
    ['cli_addr','port']

    sprintf(textbuffer, "['%s','%s']", addr, port);
    printf("%s", textbuffer);
    if (send(clientFd, textbuffer, sizeof(textbuffer), 0) < 0)
        return -1;
*/
    return 0;
}
