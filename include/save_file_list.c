#include <string.h>
#include <stdio.h>
#define MAX_BUFFER 80

/**
Zapisuje przesłaną przez klienta listę plików. Zapisuje w pliku sesji.
*/
void save_file_list(char buff[255], char client[16])
{
    char *answer;
    char file_list[MAX_BUFFER] = "";
    char filename[32];
    FILE *fh;

    answer = strtok(buff, " ");
    while ((answer = strtok(NULL, " ")) != NULL)
    {
        strcat(file_list, answer);
        sprintf(file_list, "%s\n", file_list);
    }

    sprintf(filename, "session/CLIENT_%s", client);

    /* sprawdza czy plik sesji istnieje, jezeli nie, tworzy go.. otwiera do NADPISYWANIA */
    if ((fh = fopen(filename, "w")) != NULL)
    {
        fprintf(fh, "%s", file_list);
        fclose(fh);
    }
}
