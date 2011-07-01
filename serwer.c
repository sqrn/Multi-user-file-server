/**
 * Wzajemne Udostepnianie Plikow - projekt na zaliczenie Programowanie aplikacji klient serwer
 * Mariusz Skora
 * album: 10916
 *
*/

#include "functions.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXBUF 80
#define MAXCLIENTS 10
FILE *fp;
int childCount=0;
int przetwarzaj_klienta(int clientFd, struct sockaddr_in clientaddr);
void sig_child(int s);

/**
 * sAddr - przechowuje dane adresowe serwera
 * listensock - gniazdo połączenia serwera
 * servPort - port serwera
 * result - zmienna przechowująca rezultalt wykonania funkcji koniecznych do poprawnego działania programu;
 gdy result < 0, program kończy pracę
 * pid - przechowuje wynik tworzenia procesu potomnego - funkcji fork()
 * filename - nazwa pliku; w głównej części programu wykorzystywana tylko dla pliku logowania
 *
 * Funkcja MAIN. Główna funkcja programu. Tworzenie gniazda, połączenia i odpowiadanie za procesów. Wywoływanie funkcji obsługującej potomka.
 */
int main(int argc, char *argv[])
{
    struct sockaddr_in sAddr;
    int listensock;
    int servPort;
    int result;
    int pid = 0;
    char filename[32];

    char przekroczona_liczba_klientow[] = "ERROR: Przekroczona liczba klientow";

    if (argc < 2) {
       fprintf(stderr, "Usage: %s <port>\n",argv[0]);
       exit(1);
    }

    if( (fp = fopen("log","a")) != NULL )
    {
        printf("INFO: Pomyslnie otwarto plik log.\n");
    }
    else
    {
        printf("ERROR: Nie mozna otworzyc pliku log. Zamykam.\n");
        exit(1);
    }

    listensock = socket(PF_INET, SOCK_STREAM, 0);
    if (listensock <0) {
       perror("ERROR: Nie mozna utworzyc gniazda!\n");
       exit(1);
    }

    signal(SIGCHLD,sig_child);

    servPort=atoi(argv[1]);

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(servPort);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    result = bind(listensock, (struct sockaddr *) &sAddr, sizeof(sAddr));
    if (result < 0) {
        perror("ERROR: bind serwer error.\n");
        exit(1);
    }
    result = listen(listensock, 5);
    if (result < 0) {
        perror("EROR: Listen serwer error.\n");
        exit(1);
    }

    while (1)
    {
        int sock_cli, adr_size = sizeof(sAddr);
        sock_cli = accept(listensock, (struct sockaddr *)&sAddr, (socklen_t *)&adr_size);
        if(sock_cli > 0)
        {
            if ( ( pid = fork()) == 0 )
            {
                close(listensock);
                if (childCount >= MAXCLIENTS)
                {
                    /* wyslij do klienta informacje o przekroczeniu liczby polaczen */
                    send(sock_cli,
                         przekroczona_liczba_klientow,
                         sizeof(przekroczona_liczba_klientow),
                         0);
                    /*zapisz do logu */
                    fprintf(fp, "%s", "ERROR: Przekroczona liczba klientow!\n");
                    close(sock_cli);
                    sleep(3);
                }
                /* obsluga klienta */
                if( przetwarzaj_klienta(sock_cli, sAddr) < 0 )
                {
                    /* usun plik sesji skojarzony z uzytkownikiem */
                    sprintf(filename, "session/CLIENT_%i", childCount);
                    if(remove(filename) < 0)
                    {
                        printf("WARNING: Nie usunieto pliku sesji skojarzonego z klientem!\n");
                        /* zapisz do logu */
                        fprintf(fp, "%s", "WARNING: Nie usunieto pliku sesji skojarzonego z klientem!\n");
                    }

                    printf("INFO: Zamykam potomka.\n");
                    close(sock_cli);
                    exit(0);
                }
            }
            else if(pid > 0)
            {
                childCount++;
                close(sock_cli);
            }
            else
            {
                perror("fork()"); //powrot do poczatku petli
            }
        }
    }
    fclose(fp); /*zamknij plik */
}
/*funckja systemowa, uruchamiana automatycznie */
void sig_child(int s)
{
    while ( waitpid(-1, 0, WNOHANG) > 0 )
    childCount --;
}
/**
 * Funkcja uruchamiana dla każdego oddzielnego procesu. Obsługuje przychodzącego klienta.
 * 1. Komunikacja w serwerze oparta jest na protokole TCP. W funkcji przetwarzaj_klienta() działa nieskończona pętla, która rejestruje ruch przychodzący od klientów
 łączących się z serwerem. Odpowiada za to funkcja recv(clientFd, buff, 255,0). W dalszej części za pomocą funkcji strcmp następuje porównanie zmiennej buff do obsługiwanych
 przez serwer komunikatów. Poniżej ich lista:
 HELLO - przesyła każdy nowy klient, który nawiazuje połączenie z serwerem
 FIND_FILE - za komunikatem znajduje się nazwa pliku, której szuka klient. Musi być ciągiem nie przerwanym białą linią.
 GET_FILE - za komunikatem znajduje się nazwa pliku, który klient chce pobrać. Musi być ciągiem nie przerwanym białą linią.
 SHOW_FILES - za komunikatem znajduje się lista plików, które udostępnia klient w swoim katalogu. Nazwy plików oddzielone sa białym znakiem spacji.
 BYE - komunikat kończący połączenie klienta z serwerem. Przesyła go klient w momencie zakończenia swojej pracy.
 * Zmienne:
 * klientDl - przechowuje rozmiar pola adresu serwera
 * buff - przechowuje komunikat przesyłany przez klienta
 * textbuffer - ogólna zmienna używana do przesyłania komunikatu do klienta
 * filename - nazwa pliku wskazana przez klienta
 * *msg_from_client - wskaźnik na typ char, przechowuje komunikat klienta oddzielony przez funkcję strtok
 * connNumber - przechowuje bierzący numer połączenia - id klienta
 * *fd - wskaźnik na typ FILE, jest deskryptorem pliku log
 */
int przetwarzaj_klienta(int clientFd, struct sockaddr_in clientaddr)
{
    unsigned int klientDl;
    klientDl = sizeof(clientaddr);
    char buff[255];
    char textbuffer[MAXBUF];
    char filename[32];
    char *msg_from_client;
    //char nawiazano_polaczenie[] = "Nawiazano polaczenie pomyslnie!\n";
    //char ERROR_problem_z_polaczeniem[] = "ERROR";
    char connNumber[2]; /* liczba polaczen max 99 */
    FILE *fd;
    /* -- etap klienta -
    K. Klient probuje nawiazac polaczenie
    S. Serwer akceptuje polaczenie
    K. Klient przesyla powitanie - Hello.
    S. Serwer sprawdza, czy klient sie przywital i przesyla do niego numer polaczenia. Dalej zezwala na dalszy proces.
    K. Klient otrzymuje numer polaczenia od serwera. Od teraz mozliwe przesylanie plikow.
    -- etap klienta -- */
    //if (recvfrom(clientFd, buff, 255, 0, (struct sockaddr *)&clientaddr, &klientDl) > 0)
    while(1)
    {
        if( (fp = fopen("log","a")) == NULL)
        {
            printf("ERROR: Nie mozna otworzyc pliku log. Zamykam serwer.\n");
            exit(1);
        }

        if (recv(clientFd, buff, 255, 0))
        {
            /* sprawdzam, czy klient wyslal powitanie */
            if (strncmp(buff, "HELLO", 5) == 0) //HELLO (5 znakow + 1 koniec znaku) = 6
            {
                sprintf(connNumber, "CLIENT_%i ", childCount);
                if(send_message(clientFd, clientaddr, connNumber) < 0 )
                {
                    printf("Wystapil blad. Nie mozna przeslac wiadomosci do klienta!\n");
                    /*zapisz do logu */
                    fprintf(fp, "%s", "ERROR: Nie mozna przeslac wiadomosci do klienta!\n");
                    /* powiadom klienta
                    send(clientFd,
                         ERROR_problem_z_polaczeniem,
                         sizeof(ERROR_problem_z_polaczeniem),
                         0);
                    */
                    return -1;
                }
                else
                {
                    /* jezeli klient sie polaczy, trzeba zapisac jego adres do pliku ! */
                    // wykorzystac funkcje getpeername()
                    if(getpeername(clientFd, (struct sockaddr *)&clientaddr, &klientDl) != 0)
                        perror("ERROR: getpeername()");
                    else
                    {
                        printf("INFO: Doszedl nowy klient: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                        sprintf(filename, "session/CLIENT_%i",childCount);
                        if((fd = fopen(filename,"a")) == NULL )
                        {
                            perror("ERROR: Nie mozna utworzyc pliku sesji! Zamykam serwer.");
                            exit(1);
                        }
                        else
                        {
                            sprintf(textbuffer, "%s", inet_ntoa(clientaddr.sin_addr));
                            fprintf(fd, "['%s']", textbuffer);
                            fclose(fd);
                        }
                    }
                    /* dodaj informacje do loga */
                    fprintf(fp, "INFO: Nawiazano nowe polaczenie!\n");
                }
            }
            /* Sprawdzanie czy plik istnieje */
            if (strncmp(buff, "FIND_FILE ", 10) == 0)
            {
                msg_from_client = strtok(buff," ");
                msg_from_client = strtok(NULL," ");

                sprintf(filename, "%s", msg_from_client);
                /* zapisz info do logu */
                sprintf(textbuffer, "INFO: Klient szuka plik: %s\n", filename);
                fprintf(fp, "%s", textbuffer);

                /* Przesylanie pliku do klienta. Jezeli sie nie powiedzie, klient otrzymuje komunikat. */
                if(find_file(clientFd, clientaddr, filename) < 0)
                {
                    sprintf(textbuffer, "%s", "Nie mozna odnalezc pliku!");
                    if(send_message(clientFd, clientaddr, textbuffer) < 0)
                        fprintf(fp, "%s", "ERROR: Plik nie odnaleziony. Nie mozna poinformowac klienta!\n");
                }
                else
                {
                    sprintf(textbuffer, "%s", "Plik odnaleziony!");
                    if(send_message(clientFd, clientaddr, textbuffer) < 0)
                    {
                        /* Jezeli wystapi blad, aby klienta poinformowac, zapisz informacje w logu */
                        sprintf(textbuffer, "WARNING: Plik %s odnaleziony. Klient nie zostal poinformowany.", filename);
                        fprintf(fp, "%s", textbuffer);
                    }
                }
            }
            /* przychodzi zadanie od klienta - klient chce otrzymac plik */
            if (strncmp(buff, "GET_FILE ",9) == 0)
            {
                msg_from_client = strtok(buff," ");
                msg_from_client = strtok(NULL," ");
                sprintf(filename, "%s", msg_from_client);

                if( send_file(clientFd, clientaddr, filename) < 0)
                {
                    perror("ERROR: Nie mozna przeslac pliku. Plik nie istnieje lub inny powod!\n");
                    sprintf(textbuffer, "%s","Nie mozna przeslac pliku.");
                    if( send_message(clientFd, clientaddr, textbuffer) < 0)
                    {
                        fprintf(fp, "%s", "ERROR: Nie mozna przeslac pliku. Plik nie istnieje lub inny powod.\n");
                    }
                }

            }
            /* klient przeslal swoja nowa liste plikow.. zapisz ja */
            if (strncmp(buff, "SHOW_FILES ", 11) == 0)
            {
                printf("INFO: Klient przeslal liste plikow.\n");
                sprintf(textbuffer, "%i", childCount);
                save_file_list(buff, textbuffer);

            }
            /* jezeli klient zakonczy prace i wysle komunikat "BYE", wylacz watek dla niego */
            if (strncmp(buff, "BYE", 3) == 0)
            {
                printf("INFO: CLIENT_%i rozlaczyl sie.\n", childCount);
                return -1;
            }
        }
    fclose(fp);
    }
    return 0;
}
