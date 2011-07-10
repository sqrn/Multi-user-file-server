#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUFFER 80
#define MAXCLIENTS 10

/*!
\file find_file.c
Funkcja przeszukuje pliki sesji połączonych użytkowników w celu znalezienia szukanego przez klienta wysyłającego zapytanie - pliku.
Jeżeli funkcja odnajdzie wskazany plik - zwraca ID klienta, który plik posiada.
W innym razie funkcja zwraca -1.
\retval -1 jeżeli plik nie zostanie odnaleziony
\retval 0 jeżeli plik został odnaleziony
**/
int find_file(int clientFd, struct sockaddr_in clientaddr, char filename[32])
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
