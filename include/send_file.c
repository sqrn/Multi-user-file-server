#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUFFER 80
#define MAXCLIENTS 10


/**
Funkcja przesyla do klienta wskazany przez niego plik.
ETAPY:
1. Przeszukuje baze plikow sesji polaczonych obecnie klientow.
2. Jezeli serwer znajdzie plik w bazie plikow sesji podłączonych użytkowników, przesyła do klienta dane adresowe klienta, który szukany plik posiada.
3. Jeżeli plik nie zostanie znaleziony, funkcja zwraca -1.
4. Po poprawnym ukończeniu przesyłania danych, funkcja zwraca 0.
*/
int find_users_file(int clientFd, struct sockaddr_in clientaddr, char filename[32]);
int send_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
{
    //char textbuffer[MAX_BUFFER+1];
    //char cli_port[20];
    //char cli_addr[20];
    int cli_id;
    FILE *fh;
    char filepath[32];
    char linia[32];
    char textbuffer[MAX_BUFFER];

    printf("INFO: Klient chce otrzymac plik: %s\n", filename);
    /* Dopisanie do funkcji nazwy folderu przeszukiwania */
    cli_id = find_users_file(clientFd, clientaddr, filename);
    if(cli_id < 0)
    {
        return -1;
    }

    printf("INFO: Przesylam adres IP klienta o ID: CLIENT_%i.\n", cli_id);
    /* przesylac do klienta dane adresowe klienta, ktory posiada plik */
    /*    przeslac krotke do klienta: ['cli_addr','port'] */

    sprintf(filepath, "session/CLIENT_%i",cli_id);
    if( (fh = fopen(filepath, "r")) != NULL)
    {
        if(fgets(linia, 32, fh) != NULL)
        {
            sprintf(textbuffer,"%s",linia);
            if(send(clientFd, textbuffer, sizeof(textbuffer), 0) < 0)
                return -1;
            else
                return 0;
        }
    }
    return -1;
}
int find_users_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
{
    FILE *fh;
    char filepath[32];
    printf("INFO: Szukam pliku: %s\n", filename);
    char word[MAX_BUFFER];
    int i=0;

    while(i<MAXCLIENTS)
    {
        sprintf(filepath, "session/CLIENT_%i", i);
        if ((fh = fopen(filepath, "r")) != NULL)
        {
            printf("INFO: Szukam w pliku: %s\n", filepath);
            while (fscanf(fh, "%s", word) == 1)
            {
                if(strncmp(word, filename, strlen(filename)) == 0 )
                {
                    fclose(fh);
                    printf("INFO: Plik posiada klient o ID: CLIENT_%i\n",i);
                    /* zwraca numer klienta, ktorzy posiada szukany plik - conajmniej 0 */
                    return i;
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
