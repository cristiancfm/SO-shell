// Cristian Ferreiro Montoiro - cristian.ferreiro
// Xoel González Pereira - xoel.gonzalezp

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define MAX 1024


//TYPES ---------------------------------------------------------------------
#define LNULL -1
#define LMAX 4096


typedef char tCmdName[MAX];
typedef int tPosL;

typedef struct tItemL{
    tCmdName cmdName;
} tItemL;

typedef struct {
    tItemL data[LMAX];
    tPosL lastPos;
} tList;

//FUNCTIONS DECLARATION ------------------------------------------------------

void createEmptyList(tList* L);
tPosL next(tPosL p, tList* L);
tPosL first(tList* L);
tPosL last(tList* L);
bool insertItem(tItemL d, tPosL p, tList* L);
tItemL getItem(tPosL p, tList* L);
void deleteList(tList* L);

void printAllListCommands(tList* L);
void printNListCommands(int cmdNumber, tList* L);
tItemL getListCommand(int cmdNumber, tList* L);

void cmdAuthors(char* argument);
void cmdGetPid(char* argument);
void cmdPwd(char* argument);
void cmdChdir(char * argument);
void cmdDate(char* argument);
void cmdTime(char* argument);
void cmdHistoric(char* argument, char* chain, int* repeatCounter, bool* leave, tList* L);

void readInput(char* chain, tList* L);
int splitChain(char* chain, char* piece[]);
void processInput(char* chain, bool* leave, int* repeatCounter, tList* L);

//---------------------------------------------------------------------------


//STATIC LIST FUNCTIONS -------------------------------------------------------

void createEmptyList(tList* L)
{
    L->lastPos = LNULL;
}

tPosL next(tPosL p, tList* L)
{
    if(p==L->lastPos)
        return LNULL;
    else
        return (p+1);
}

tPosL first(tList* L)
{
    if (L->lastPos != LNULL)
        return 0;
    else
        return LNULL;
}

tPosL last(tList* L)
{
    if (L->lastPos != LNULL)
        return L->lastPos;
    else
        return LNULL;
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

tItemL getItem(tPosL p, tList* L)
{
    return (L->data[p]);
}

void deleteList(tList* L)
{
    L->lastPos = LNULL;
}

void printAllListCommands(tList* L){
    tPosL i;
    for(i = first(L); i != LNULL; i = next(i, L)){
        printf("%d: %s", i, getItem(i, L).cmdName);
    }
}

void printNListCommands(int cmdNumber, tList* L){
    tPosL i;
    int iterationsCounter;

    if(cmdNumber < 0) {
        printf("Error: impossible to print %d commands\n", cmdNumber);
    }
    else {
        iterationsCounter = 0;
        i = first(L);
        while(iterationsCounter < cmdNumber && i != LNULL) {
            printf("%d: %s", i, getItem(i, L).cmdName);
            i = next(i, L);
            iterationsCounter++;
        }
    }
}

tItemL getListCommand(int cmdNumber, tList* L){
    tPosL i;
    int iterationsCounter;

    if(cmdNumber < 0) {
        printf("Error: impossible to get command number %d\n", cmdNumber);
        return getItem(LNULL, L);
    }

    else {
        iterationsCounter = 0;
        i = first(L);
        while(iterationsCounter < cmdNumber && i != LNULL) {
            i = next(i, L);
            iterationsCounter++;
        }
        return getItem(i, L);
    }
}

//-----------------------------------------------------------

//CMD FUNCTIONS ---------------------------------------------

void cmdAuthors(char* argument) {
    if (argument == NULL) {
        printf("Cristian Ferreiro Montoiro: cristian.ferreiro\n");
        printf("Xoel González Pereira: xoel.gonzalezp\n");

    } else if (!strcmp(argument, "-n")) {
        printf("cristian.ferreiro\n");
        printf("xoel.gonzalezp\n");

    } else if (!strcmp(argument, "-l")) {
        printf("Cristian Ferreiro Montoiro\n");
        printf("Xoel González Pereira\n");

    } else
        printf(" %s : unknown command argument\n", argument);

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

void cmdChdir(char * argument){

    char directory[MAX];

    if(argument == NULL){

        printf(" %s\n",getcwd(directory, MAX));

    }


    else
    {
        if (chdir(argument) != 0){
            perror("Error");
        }
    }
}

void cmdDate(char* argument){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Date: %02d-%02d-%02d \n",tm.tm_mday , tm.tm_mon + 1, tm.tm_year + 1900);

}

void cmdTime(char* argument){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Time: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void cmdHistoric(char* argument, char* chain, int* repeatCounter, bool* leave, tList* L){

    if (argument == NULL)
        printAllListCommands(L);

    else if (strcmp(argument, "-c") == 0)
        deleteList(L);

    else if (strncmp(argument, "-r", 2) == 0) {
        tPosL cmdNumber;
        tItemL cmdData;

        cmdNumber = atoi(strtok(argument, "-r"));
        cmdData = getListCommand(cmdNumber, L);
        strcpy(chain, cmdData.cmdName);

        if(cmdNumber > last(L))
            printf("Error: there is no command %d saved in the historic\n", cmdNumber);

        else if(*repeatCounter > 1) {
            printf("Error: the written command accesses the historic two times or more\n");
        }

        else {
            printf("Executing historic command %d: %s", cmdNumber, chain);
            (*repeatCounter)++;
            processInput(chain, leave, repeatCounter, L);
        }
    }

    else {
        tPosL cmdNumber;
        cmdNumber = atoi(strtok(argument, "-"));
        printNListCommands(cmdNumber, L);
    }
}

//-----------------------------------------------------------

//INPUT PROCESSING FUNCTIONS --------------------------------

void readInput(char* chain, tList* L) {
    tItemL cmdData;
    fgets(chain,MAX,stdin);
    strcpy(cmdData.cmdName, chain);
    if(!insertItem(cmdData, LNULL, L))
        printf("Error while inserting the current command in the historic list");

}

int splitChain(char* chain, char* piece[]) {
    int i = 1;
    if ((piece[0] = strtok(chain," \n\t")) == NULL) //strtok coge la cadena y la trocea si encuentra \n o \t
        return 0;
    while ((piece[i] = strtok(NULL, " \n\t")) != NULL) //las siguientes llamadas a strtok pasan NULL como primer argumento
        i++;
    return i;
}

void processInput(char* chain, bool* leave, int* repeatCounter, tList* L) {
    char* piece[MAX];

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
        cmdHistoric(piece[1], chain, repeatCounter, leave, L);

    else if(!strcmp(piece[0], "quit") || !strcmp(piece[0],"end") || !strcmp(piece[0], "exit"))
        *leave = true;

    else printf("%s: not found\n", piece[0]);
}



int main() {
    char chain[MAX];
    bool leave;
    int repeatCounter;
    leave = false;
    repeatCounter = 0;
    tList L;
    createEmptyList(&L);

    while(1){
        printf("> ");
        readInput(chain, &L);
        processInput(chain, &leave, &repeatCounter, &L);
        repeatCounter = 0;
        if(leave == true)
            break;
    }
    return 0;
}