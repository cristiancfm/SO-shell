// Cristian Ferreiro Montoiro - cristian.ferreiro
// Xoel González Pereira - xoel.gonzalezp

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX 1024


//STATIC LIST( CREATING THE ARRAY) -------------------------------------------------------

#define LNULL -1
#define LMAX 4096


//PARA CADA POSICION DE LA LISTA, ASIGNAMOS EL NOMBRE DEL ARCHIVO Y LA POSICION QUE OCUPA
typedef char tItemL[MAX];
typedef int tPosL;
typedef struct {
    tItemL data;
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





//-----------------------------------------------------------

int readInput(char* chain, tList* L) {
    fgets(chain,MAX,stdin);
    insertItem(chain, next(last(*L), *L), L);
}

int splitChain(char* chain, char* piece[]) {
    int i = 1;
    if ((piece[0] = strtok(chain,"\n\t")) == NULL) //strtok coge la cadena y la trocea si encuentra \n o \t
        return 0;
    while ((piece[i] = strtok(NULL, "\n\t")) != NULL) //las siguientes llamadas a strtok pasan NULL como primer argumento
        i++;
    return i;
}

void processInput(char* chain) {
    char* piece[MAX];

    if (splitChain(chain, piece) == 0) //no se han introducido palabras
        return;

    if (splitChain(chain, piece) == 0) //no se han introducido palabras
        return;

    if (!strcmp(piece[0], "authors"))
        cmdAuthors(piece[1]);


    else if (!strcmp(piece[0], "getpid"))
        cmdGetpid(piece[1]);

    else if(!strcmp(piece[0], "getppid"))
        cmdGetppid(piece[1]);

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
        *leave = 1;


    else printf("%s: not found\n", piece[0]);
}



int main() {
    char chain[MAX];
    tList L;
    createEmptyList(&L);

    while(true){
        printf("> ");
        readInput(&chain, &L);
        processInput(&chain);
    }


    return 0;
}



