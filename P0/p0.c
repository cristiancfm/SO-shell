// Cristian Ferreiro Montoiro - cristian.ferreiro
// Xoel González Pereira - xoel.gonzalezp

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX 1024


//STATIC LIST (ARRAY) -------------------------------------------------------

#define LNULL -1
#define LMAX 4096


//PARA CADA POSICION DE LA LISTA, ASIGNAMOS EL NOMBRE DEL ARCHIVO Y LA POSICION QUE OCUPA
typedef char tCmdName[MAX];
typedef int tPosL;

typedef struct tItemL{
    tCmdName cmdName;
} tItemL;

typedef struct {
    tItemL data[LMAX];
    tPosL lastPos;
} tList;




void createEmptyList(tList* L)
{
    L->lastPos = LNULL;
}


tPosL next (tPosL p, tList L)
{
    if(p==L.lastPos)
        return LNULL;
    else
        return (p+1);
}


tPosL last(tList L)
{
    return L.lastPos;
}


bool insertItem(tItemL d, tPosL p, tList* L)
{
    tPosL i;
    if (L->lastPos >= LMAX - 1)
        return false;
    else
    {
        L->lastPos++;
        if (p == LNULL)
        {
            L->data[L->lastPos] = d;
            /* Since we updated the last position before,
            * the item will be inserted after the last item
            * of the list */
        }
        else
        {
            for (i = L->lastPos; i >= p+1; i--)
            {
                L->data[i] = L->data[i-1];
            }
            L->data[p] = d;
        }
        return true;
    }
}

//-----------------------------------------------------------

//CMD FUNCTIONS ---------------------------------------------

void cmdAuthors ( char * argument) {

    if (argument == NULL) {

        printf("Cristian Ferreiro Montoiro: cristian.ferreiro\n");
        printf("Xoel González Pereira: xoel.gonzalezp\n");

    } else {

        if (!strcmp(argument, "n")) {


            printf("cristian.ferreiro\n");
            printf("xoel.gonzalezp\n");

        } else if (!strcmp(argument, "l")) {

            printf("Cristian Ferreiro Montoiro\n");
            printf("Xoel González Pereira\n");

        } else {

            printf(" %s : unknown command argument\n", argument);

        }

    }
}



void cmdGetPid(char* argument){
    printf("Shell PID: %d\n", getpid());
}

void cmdGetPPid(char* argument){
    printf("Shell Parent PID: %d\n", getppid());
}

void cmdPwd(char* argument){
    char directory[MAX];
    printf(" %s\n",getcwd(directory, MAX));
}


void cmdDate(char* argument){


}

//-----------------------------------------------------------

//INPUT PROCESSING FUNCTIONS --------------------------------

void readInput(char* chain, tList* L) {
    tItemL cmdData;
    fgets(chain,MAX,stdin);
    strcpy(cmdData.cmdName, chain);
    insertItem(cmdData, next(last(*L), *L), L);
}

int splitChain(char* chain, char* piece[]) {
    int i = 1;
    if ((piece[0] = strtok(chain,"\n\t")) == NULL) //strtok coge la cadena y la trocea si encuentra \n o \t
        return 0;
    while ((piece[i] = strtok(NULL, "\n\t")) != NULL) //las siguientes llamadas a strtok pasan NULL como primer argumento
        i++;
    return i;
}

void processInput(char* chain, bool* leave) {
    char* piece[MAX];

    if (splitChain(chain, piece) == 0) //no se han introducido palabras
        return;

    if (splitChain(chain, piece) == 0) //no se han introducido palabras
        return;

    if (!strcmp(piece[0], "authors"))
        cmdAuthors(piece[1]);


    else if (!strcmp(piece[0], "getpid"))
        cmdGetPid(piece[1]);

    else if(!strcmp(piece[0], "getppid"))
        cmdGetPPid(piece[1]);

    else if (!strcmp(piece[0], "pwd"))
        cmdPwd(piece[1]);

    else if (!strcmp(piece[0], "chdir"))
        cmdChdir(piece[1]);


    else if(!strcmp(piece[0], "date"))
        cmdDate(piece[1]);


    else if(!strcmp(piece[0], "time"))
        cmdTime(piece[1]);


    else if (!strcmp(piece[0], "historic"))
        cmdHistoric(piece[1]);


    else if(!strcmp(piece[0], "quit") || !strcmp(piece[0],"end") || !strcmp(piece[0], "exit"))
        *leave = true;


    else printf("%s: not found\n", piece[0]);
}



int main() {
    char chain[MAX];
    bool leave;
    leave = false;
    tList L;
    createEmptyList(&L);

    while(true){
        printf("> ");
        readInput(chain, &L);
        processInput(chain, &leave);
        if(leave == true)
            break;
    }
    return 0;
}



