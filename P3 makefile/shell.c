// Cristian Ferreiro Montoiro - cristian.ferreiro
// Xoel González Pereira - xoel.gonzalezp

#include "list.h"
#include "bgproclist.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#define MAX 1024
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

bool quit = false;

//global variables for "memory -show-vars"
int global1;
int global2;
int global3;

//TYPES ---------------------------------------------------------------------
#define LNULL (-1)
#define LMAX 4096


typedef char tName[MAX];
typedef int tPosL;

typedef struct tItemL{
    tName name;
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
void cmdGetPPid(char* argument);
void cmdPwd(char* argument);
void cmdChdir(char * argument);
void cmdDate(char* argument);
void cmdTime(char* argument);
void cmdHistoric(char* argument, char* chain, int* repeatCounter, tList* L, tListMem* M, tListProc* P);


void cmdCreate(char* argument1, char* argument2, int chainWordsCount);

void deleteDirRec(char* pathname);
void cmdDelete(char* pieces[MAX], int chainWords);

void print_perms(mode_t st);
void print_long_format(struct dirent* file, struct stat state, char* currentFilePath, bool calledByListName);
void listDir(char* pathname, bool longOption, bool hidOption);
void listDirRec(char* pathname, bool longOption, bool hidOption);
void listName(char* name, bool longOption);
void cmdList(char* pieces[MAX], int chainWords);


void memoryAllocateMalloc(char* pieces[MAX], int chainWords, tListMem* M);
void memoryAllocateMmap(char* pieces[MAX], int chainWords, tListMem* M);
void * ObtenerMemoriaShmget (key_t clave, size_t tam, tItemM* newEntry);
void memoryAllocateCreateShared(char* pieces[MAX], int chainWords, tListMem* M);
void memoryAllocateShared(char* pieces[MAX], int chainWords, tListMem* M);
void memoryAllocate(char* pieces[MAX], int chainWords, tListMem* M);

void memoryDeallocMalloc(char* pieces[MAX], int chainWords, tListMem* M);
void memoryDeallocMmap(char* pieces[MAX], int chainWords, tListMem* M);
void memoryDeallocShared(char* pieces[MAX], int chainWords, tListMem* M);
void memoryDeallocAddress(char* pieces[MAX], int chainWords, tListMem* M);
void memoryDealloc(char* pieces[MAX], int chainWords, tListMem* M);

void memoryDeleteKey(char* pieces[MAX], int chainWords);

void memoryShow(char* pieces[MAX], int chainWords, tListMem* M);
void memoryShowVars();
void memoryShowFuncs();

void memoryDoPmap();

void cmdMemory(char* pieces[MAX], int chainWords, tListMem* M);
void cmdMemDump(char* pieces[MAX], int chainWords);
void cmdMemFill(char* pieces[MAX], int chainWords);

void doRecursiva (int n);
void cmdRecurse(char* pieces[MAX], int chainWords);

void cmdReadFile(char* pieces[MAX], int chainWords);
void cmdWriteFile(char* pieces[MAX], int chainWords);


void cmdGetPriority(char* pieces[MAX], int chainWords);
void cmdSetPriority(char* pieces[MAX], int chainWords);
char* NombreUsuario (uid_t uid);
uid_t UidUsuario (char * nombre);
void cmdGetUid();
int cmdSetUid(char* pieces[MAX], int chainWords);
void cmdFork();
void cmdExecute(char* pieces[MAX], int chainWords);
void cmdForeground(char* pieces[MAX], int chainWords);
void cmdBackground(char* pieces[MAX], int chainWords, tListProc* P);
void cmdRunAs(char* pieces[MAX], int chainWords, tListProc* P);
void cmdExecuteAs(char* pieces[MAX], int chainWords, tListProc* P);
void cmdOther(char* pieces[MAX], int chainWords, tListProc* P);
void cmdProc(char* pieces[MAX], int chainWords, tListProc* P);
void cmdDeleteProcs(char* pieces[MAX], int chainWords, tListProc* P);


void readInput(char* chain, tList* L);
int splitChain(char* chain, char* pieces[]);
void processInput(char* chain, int* repeatCounter, tList* L, tListMem* M, tListProc* P);

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
        printf("%d: %s", i, getItem(i, L).name);
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
            printf("%d: %s", i, getItem(i, L).name);
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

void cmdChdir(char* argument){

    char directory[MAX];

    if(argument == NULL){

        printf(" %s\n",getcwd(directory, MAX));

    }


    else
    {
        if (chdir(argument) != 0){
            printf("Error: %s\n", strerror(errno));
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

void cmdHistoric(char* argument, char* chain, int* repeatCounter, tList* L, tListMem* M, tListProc* P){

    if (argument == NULL)
        printAllListCommands(L);

    else if (strcmp(argument, "-c") == 0)
        deleteList(L);

    else if (strncmp(argument, "-r", 2) == 0) {
        tPosL cmdNumber;
        tItemL cmdData;

        cmdNumber = atoi(strtok(argument, "-r"));
        cmdData = getListCommand(cmdNumber, L);
        strcpy(chain, cmdData.name);

        if(cmdNumber > last(L))
            printf("Error: there is no command %d saved in the historic\n", cmdNumber);

        else if(*repeatCounter > 1) {
            printf("Error: the written command accesses the historic two times or more\n");
        }

        else {
            printf("Executing historic command %d: %s", cmdNumber, chain);
            (*repeatCounter)++;
            processInput(chain, repeatCounter, L, M, P);
        }
    }

    else {
        tPosL cmdNumber;
        cmdNumber = atoi(strtok(argument, "-"));
        printNListCommands(cmdNumber, L);
    }
}

//-----------------------------------------------------------

void cmdCreate(char* argument1, char* argument2, int chainWordsCount){
    if(chainWordsCount == 1) {
        //create: llamar a list
        listDir(".", false, false);
    }

    else if(chainWordsCount == 2) {
        if(strncmp(argument1, "-dir", 4) == 0){
            //create -dir: llamar a list
            listDir(".", false, false);
        }
        else{
            char* pathname = argument1;
            int fd; //file descriptor
            fd = open(pathname, O_CREAT, 0665); // 0665 = user: read, write; group: read, write; others: read

            if (fd == -1){
                printf("Error: %s\n", strerror(errno));
            }
            else{
                printf("File %s created\n", pathname);
            }
        }
    }

    else if(chainWordsCount == 3) {
        if (strncmp(argument1, "-dir", 4) == 0) {
            char *pathname = argument2;
            int fd; //file descriptor
            fd = mkdir(pathname, 0775);
            // 0775 = user: create, delete files; group: create, delete files; others: access files

            if (fd == -1) {
                printf("Error: %s\n", strerror(errno));
            } else {
                printf("Directory %s created\n", pathname);
            }
        }
    }

    else {
        printf("Error: too many arguments.\n");
    }

}

void deleteDirRec(char* pathname){

    DIR* directory = NULL;
    struct dirent* file = NULL;
    struct stat state;
    char currentFilePath[MAX];

    directory = opendir(pathname); //abre el directorio actual

    if(directory != NULL){

        while ((file = readdir(directory)) != NULL){ //lee el contenido del directorio
            sprintf(currentFilePath, "%s/%s", pathname, file->d_name); //guarda la ruta "directory/file"

            if(lstat(currentFilePath, &state) == 0){
                //listar directorio actual y borrar contenidos recursivamente

                if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                    continue;
                }
                else{
                    if(file->d_type == DT_DIR){ //el archivo actual es un directorio
                        deleteDirRec(currentFilePath);
                    }
                    else{ //es un archivo normal
                        if(remove(currentFilePath) != 0){
                            printf("Error: %s\n", strerror(errno));
                        }
                    }
                }
            }
            else{
                printf("Error: %s\n", strerror(errno));
            }
        }

        if(remove(pathname) != 0){
            printf("Error: %s\n", strerror(errno));
        }

        closedir(directory);

    }
    else{
        printf("Error deleting %s: %s\n", pathname, strerror(errno));
    }

}

void cmdDelete(char* pieces[MAX], int chainWords) {

    DIR* directory = NULL;
    bool recOption;
    int optionWords = 0;
    //optionWords: no. of options for the command (example: "list -dir -hid /home" has optionWords = 2 )
    int i;

    for (i = 0; i <= chainWords - 1; i++) {
        if(!strcmp(pieces[i], "-rec")){
            recOption = true;
            optionWords++;
        }
    }

    if(optionWords == chainWords - 1){ //no se han especificado directorios: listar directorio actual
        listDir(".", false, false);
    }
    else {
        // remove()  deletes  a  name from the filesystem.  It calls unlink(2) for
        //       files, and rmdir(2) for directories.
        //
        // rmdir() deletes a directory, which must be empty.

        for (i = optionWords + 1; i <= chainWords - 1; i++) {

            if ((directory = opendir(pieces[i])) != NULL) { //es un directorio

                if (recOption) { //borrar archivos y directorios con sus contenidos especificados
                    deleteDirRec(pieces[i]);
                }
                else { //borrar archivos y directorios vacíos especificados
                    if (remove(pieces[i]) != 0) {
                        printf("Error: %s\n", strerror(errno));
                    }
                }

                closedir(directory);
            }
            else { //es un archivo
                if (remove(pieces[i]) != 0) {
                    printf("Error: %s\n", strerror(errno));
                }

            }

        }
    }
}



void print_perms(mode_t st) {
    char perms[11];
    if(st && S_ISREG(st)) perms[0]='-';
    else if(st && S_ISDIR(st)) perms[0]='d';
    else if(st && S_ISFIFO(st)) perms[0]='|';
    else if(st && S_ISSOCK(st)) perms[0]='s';
    else if(st && S_ISCHR(st)) perms[0]='c';
    else if(st && S_ISBLK(st)) perms[0]='b';
    else perms[0]='l';  // S_ISLNK
    perms[1] = (st & S_IRUSR) ? 'r':'-';
    perms[2] = (st & S_IWUSR) ? 'w':'-';
    perms[3] = (st & S_IXUSR) ? 'x':'-';
    perms[4] = (st & S_IRGRP) ? 'r':'-';
    perms[5] = (st & S_IWGRP) ? 'w':'-';
    perms[6] = (st & S_IXGRP) ? 'x':'-';
    perms[7] = (st & S_IROTH) ? 'r':'-';
    perms[8] = (st & S_IWOTH) ? 'w':'-';
    perms[9] = (st && S_IXOTH) ? 'x':'-';
    perms[10] = '\0';
    printf(" %s ", perms);
}

void print_long_format(struct dirent* file, struct stat state, char* currentFilePath, bool calledByListName){

    //Time
    struct tm time;
    char datestring[256];

    localtime_r(&state.st_mtime, &time);
    strftime(datestring, sizeof(datestring), "%b %d %H:%M", &time);
    printf("%s  ", datestring);

    //Inode
    printf("%-8lu ", state.st_ino);

    //Owner
    struct passwd* owner;
    owner = getpwuid(state.st_uid);

    if(owner == NULL){
        printf("--- ");
    }
    else{
        printf("%s ", owner->pw_name);
    }

    //Group
    struct passwd* group;
    group = getpwuid(state.st_gid);

    if(group == NULL){
        printf("--- ");
    }
    else{
        printf("%s ", group->pw_name);
    }

    //Mode
    print_perms(state.st_mode);

    //Size
    printf("%8ld ", state.st_size);

    //Number of links
    printf("(%3lu) ", state.st_nlink);

    //Name
    if(calledByListName){
        printf("%s", currentFilePath);
        //the whole path of the directory or the name of the file will be printed
    }
    else{
        printf("%s", file->d_name);
        //only the name of the directory or file will be printed
    }


    //Symbolic link
    if(S_ISLNK(state.st_mode)){
        enum { BUFFERSIZE = 1024 };
        char buf[BUFFERSIZE];
        ssize_t len = readlink(currentFilePath, buf, sizeof(buf)-1);

        if (len != -1) {
            buf[len] = '\0';
            printf(" -> %s", buf);
        }
        else {
            printf("Error: %s\n", strerror(errno));
        }
    }

    //Print error if owner or group are NULL
    if(owner == NULL || group == NULL){
        printf("\nError: Impossible to get the owner or the group of the file");
    }

    printf("\n");
}

void listDir(char* pathname, bool longOption, bool hidOption){

    DIR* directory = NULL;
    struct dirent* file = NULL;
    struct stat state;
    char currentFilePath[MAX];

    directory = opendir(pathname); //abre el directorio actual

    if(directory != NULL){
        printf("****** %s\n", pathname);

        while ((file = readdir(directory)) != NULL){ //lee el contenido del directorio
            sprintf(currentFilePath, "%s/%s", pathname, file->d_name); //guarda la ruta "directory/file"

            if(lstat(currentFilePath, &state) == 0){

                if(hidOption){
                    //listar directorio actual con archivos ocultos

                    if(!longOption){
                        printf("%-10d %s \n", (int)state.st_size, file->d_name);
                    }
                    else{
                        print_long_format(file, state, currentFilePath, false);
                    }

                }
                else{
                    //listar directorio actual

                    if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0
                       || file->d_name[0] == '.'){
                        continue;
                    }
                    else{
                        if(!longOption){
                            printf("%-10d %s \n", (int)state.st_size, file->d_name);
                        }
                        else{
                            print_long_format(file, state, currentFilePath, false);
                        }

                    }
                }

            }
            else{
                printf("Error: %s\n", strerror(errno));
            }
        }

    }
    else{
        printf("Error listing %s: %s\n", pathname, strerror(errno));
    }

    closedir(directory);

}

void listDirRec(char* pathname, bool longOption, bool hidOption){

    DIR* directory = NULL;
    struct dirent* file = NULL;
    struct stat state;
    char currentFilePath[MAX];

    directory = opendir(pathname); //abre el directorio actual

    if(directory != NULL){

        listDir(pathname, longOption, hidOption);

        while ((file = readdir(directory)) != NULL){ //lee el contenido del directorio
            sprintf(currentFilePath, "%s/%s", pathname, file->d_name); //guarda la ruta "directory/file"

            if(lstat(currentFilePath, &state) == 0){

                if(S_ISDIR(state.st_mode)){ //comprueba que el archivo actual es un directorio

                    if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                        continue;
                    }
                    else{
                        listDirRec(currentFilePath, longOption, hidOption);
                    }

                }
            }
            else{
                printf("Error: %s\n", strerror(errno));
            }
        }

    }
    else{
        printf("Error listing %s: %s\n", pathname, strerror(errno));
    }

    closedir(directory);

}

void listName(char* name, bool longOption){
    DIR* directory = NULL;
    struct dirent* file = NULL;
    struct stat state;
    char currentFilePath[MAX];
    bool fileFound = false;

    if (name[0] == '/'){ //es un directorio: abirlo y listarlo

        directory = opendir(name); //abrir el directorio

        if(directory != NULL){
            printf("------\n");

            while ((file = readdir(directory)) != NULL){ //lee el contenido del directorio
                sprintf(currentFilePath, "%s/%s", name, file->d_name); //guarda la ruta "directory/file"

                if(lstat(currentFilePath, &state) == 0){

                    if(strcmp(file->d_name, ".") == 0) {
                        //imprime solo el directorio abierto

                        if(!longOption){
                            printf("%-10d %s \n", (int)state.st_size, name);
                        }
                        else{
                            print_long_format(file, state, name, true);
                        }

                    }
                }
                else{
                    printf("Error: %s\n", strerror(errno));
                }
            }
        }
        else{
            printf("Error listing %s: %s\n", name, strerror(errno));
        }

        closedir(directory);

    }
    else{ //es un archivo: abrir el directorio actual y listar el archivo

        directory = opendir("."); //abrir el directorio actual

        if(directory != NULL){
            printf("------\n");

            while ((file = readdir(directory)) != NULL){ //lee el contenido del directorio
                sprintf(currentFilePath, "./%s", file->d_name); //guarda la ruta "./file"

                if(lstat(currentFilePath, &state) == 0){

                    if(strcmp(file->d_name, name) == 0) {
                        //imprime solo el archivo especificado del directorio abierto

                        fileFound = true;

                        if(!longOption){
                            printf("%-10d %s \n", (int)state.st_size, name);
                        }
                        else{
                            print_long_format(file, state, name, true);
                        }

                    }
                }
                else{
                    printf("Error: %s\n", strerror(errno));
                }
            }

            if(!fileFound){ //no hay ningún archivo con el nombre especificado en el directorio actual
                printf("Error: file not found\n");
            }
        }
        else{
            printf("Error listing %s: %s\n", name, strerror(errno));
        }

        closedir(directory);

    }

}

void cmdList(char* pieces[MAX], int chainWords){
    bool longOption = false;
    bool dirOption = false;
    bool hidOption = false;
    bool recOption = false;

    int optionWords = 0;
    //optionWords: no. of options for the command (example: "list -dir -hid /home" has optionWords = 2 )

    int i;

    for (i = 0; i <= chainWords - 1; i++){
        if(!strcmp(pieces[i], "-long")){
            longOption = true;
            optionWords++;
        }
        if(!strncmp(pieces[i], "-dir", 4)){
            dirOption = true;
            optionWords++;
        }
        if(!strncmp(pieces[i], "-hid", 4)){
            hidOption = true;
            optionWords++;
        }
        if(!strncmp(pieces[i], "-rec", 4)){
            recOption = true;
            optionWords++;
        }
    }


    if(optionWords == chainWords - 1){ //no se han especificado directorios

        if(recOption){
            listDirRec(".", longOption, hidOption);
        }
        else{
            listDir(".", longOption, hidOption);
        }

    }
    else{

        if(dirOption){ //listar archivos y contenido de directorios especificados

            for(i = optionWords + 1; i <= chainWords - 1; i++){

                if (opendir(pieces[i]) != NULL){ //es un directorio

                    if(recOption){
                        listDirRec(pieces[i], longOption, hidOption);
                    }
                    else{
                        listDir(pieces[i], longOption, hidOption);
                    }

                }
                else{
                    listName(pieces[i], longOption);
                }

            }

        }
        else{ //listar archivos y directorios (solamente sus nombres)

            for(i = optionWords + 1; i <= chainWords - 1; i++){
                listName(pieces[i], longOption);
            }
        }
    }
}

//-----------------------------------------------------------

void memoryAllocateMalloc(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 3){ //no size specified: print list elements allocated with malloc
        printMemList(1, M);
    }
    else{
        tItemM newEntry;

        newEntry.size = strtoul(pieces[3], NULL, 10);

        newEntry.memAddress = malloc(newEntry.size);

        if(newEntry.memAddress == NULL){ //malloc failed
            printf("Error: %s\n", strerror(errno));
        }
        else{
            newEntry.memAllocationType = 1; //malloc

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            newEntry.time = tm;

            insertItemMem(newEntry, LNULL, M);

            printf("%ld bytes allocated at %p\n", newEntry.size, newEntry.memAddress);
        }


    }

}

void memoryAllocateMmap(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 3){ //no file specified: print list elements allocated with mmap
        printMemList(2, M);
    }
    else{
        tItemM newEntry;
        struct stat state;
        int mode;
        int protection = 0;

        strcpy(newEntry.fileName, pieces[3]);


        if(pieces[4] == NULL){ //no permissions specified
            mode = O_RDONLY;
        }
        else{
            if(strchr(pieces[4], 'r') != NULL){ //read
                mode = O_RDONLY;    protection|=PROT_READ;
            }
            else if(strchr(pieces[4], 'w')){ //write
                mode = O_RDWR;      protection|=PROT_READ;
            }
            else if(strchr(pieces[4], 'x')){ //execute
                mode = O_RDWR;      protection|=PROT_EXEC;
            }
            else{ //invalid permissions specified
                mode = O_RDONLY;

            }
        }

        newEntry.fd = open(newEntry.fileName, mode);

        if(newEntry.fd != -1){ //file opened successfully

            if(fstat(newEntry.fd, &state) == 0) { //stat struct returned successfully

                newEntry.size = state.st_size;

                newEntry.memAddress = (long *) mmap(NULL, newEntry.size, protection,
                                                    MAP_PRIVATE, newEntry.fd, 0);

            }

        }
        else{ //error when opening the file
            printf("Error: %s\n", strerror(errno));
        }


        if(newEntry.memAddress == MAP_FAILED){ //mmap failed
            printf("Error: %s\n", strerror(errno));
        }
        else{
            newEntry.memAllocationType = 2; //mapped file

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            newEntry.time = tm;

            insertItemMem(newEntry, LNULL, M);

            printf("File %s mapped at %p\n", newEntry.fileName, newEntry.memAddress);
        }

    }

}

void * ObtenerMemoriaShmget (key_t clave, size_t tam, tItemM* newEntry)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam) /*si tam no es 0 la crea en modo exclusivo */
        flags=flags | IPC_CREAT | IPC_EXCL;
        /*si tam es 0 intenta acceder a una ya creada*/

    if (clave==IPC_PRIVATE){ /*no nos vale*/
        errno=EINVAL; return NULL;}

    if ((id=shmget(clave, tam, flags))==-1) //shmget: gets the ID of a System V shared memory segment or creates a new one
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){ //shmat: attaches the memory segment to the address space of the process
        aux=errno;  /*si se ha creado y no se puede mapear*/
        if (tam)    /*se borra */
            shmctl(id,IPC_RMID,NULL); //shmctl with IPC_RMID: marks the segment to be destroyed
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s); //shmctl with IPC_STAT: copies the info to s

    //copy info to list entry
    newEntry->size = s.shm_segsz;

    return (p);
}

void memoryAllocateCreateShared(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 3){ //no key specified: print list elements allocated with createshared
        printMemList(3, M);
    }
    else {

        if(chainWords == 4) { //no size specified: print list elements allocated with createshared
            printMemList(3, M);
        }
        else{
            tItemM newEntry;
            newEntry.memKey = 0;
            long size = 0;

            newEntry.memKey = strtoul(pieces[3], NULL, 10);

            size = strtoul(pieces[4], NULL, 10);

            newEntry.memAddress = ObtenerMemoriaShmget(newEntry.memKey, size, &newEntry);


            if(newEntry.memAddress == NULL){
                printf("Error: %s\n", strerror(errno));
            }
            else{
                newEntry.memAllocationType = 3; //shared memory

                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                newEntry.time = tm;

                insertItemMem(newEntry, LNULL, M);

                printf("Shared memory of key %d allocated in %p\n", newEntry.memKey, newEntry.memAddress);
            }
        }

    }

}

void memoryAllocateShared(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 3){ //no key specified: print list elements allocated with createshared
        printMemList(3, M);
    }
    else {

        tItemM newEntry;
        newEntry.memKey = 0;
        newEntry.size = 0;

        newEntry.memKey = strtoul(pieces[3], NULL, 10);

        newEntry.memAddress = ObtenerMemoriaShmget(newEntry.memKey, 0, &newEntry);


        if(newEntry.memAddress == NULL){
            printf("Error: %s\n", strerror(errno));
        }
        else{
            newEntry.memAllocationType = 3; //shared memory

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            newEntry.time = tm;

            insertItemMem(newEntry, LNULL, M);

            printf("Shared memory with key %d allocated at %p\n", newEntry.memKey, newEntry.memAddress);
        }

    }

}

void memoryAllocate(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 2){ //"memory -allocate": print all the list elements
        printMemList(0, M);
    }
    else{
        if(!strcmp(pieces[2], "-malloc")){
            memoryAllocateMalloc(pieces, chainWords, M);
        }
        if(!strcmp(pieces[2], "-mmap")){
            memoryAllocateMmap(pieces, chainWords, M);
        }
        if(!strcmp(pieces[2], "-createshared")){
            memoryAllocateCreateShared(pieces, chainWords, M);
        }
        if(!strcmp(pieces[2], "-shared")){
            memoryAllocateShared(pieces, chainWords, M);
        }
        
    }

}



void memoryDeallocMalloc(char* pieces[MAX], int chainWords, tListMem* M){
    if(chainWords == 3){ //no size specified: print list elements allocated with malloc
        printMemList(1, M);
    }
    else{
        tSize size;
        tPosM pos;
        tItemM item;

        size = strtoul(pieces[3], NULL, 10);

        pos = findSize(size, 1, M);

        if (pos == LNULL){ //block of specified memory not found: print list elements allocated with malloc
            printMemList(1, M);
        }
        else{
            item = getItemMem(pos, M);
            free(item.memAddress);
            printf("%ld bytes deallocated at %p (malloc)\n", item.size, item.memAddress);
            *M = deleteAtPositionMem(pos, M);
        }
    }
}

void memoryDeallocMmap(char* pieces[MAX], int chainWords, tListMem* M){
    if(chainWords == 3){ //no file specified: print list elements allocated with mmap
        printMemList(2, M);
    }
    else{
        char fileName[MAX];
        tPosM pos;
        tItemM item;

        strcpy(fileName, pieces[3]);

        pos = findFileName(fileName, 2, M);

        if (pos == LNULL){ //block of specified memory not found: print list elements allocated with mmap
            printMemList(2, M);
        }
        else{
            item = getItemMem(pos, M);
            if(munmap(item.memAddress, item.size) != 0){ //an error occurred while unmapping the file
                printf("Error: %s\n", strerror(errno));
            }
            else{
                if(close(item.fd) != 0){ //an error occurred while closing the file
                    printf("Error: %s\n", strerror(errno));
                }
                else{
                    printf("File %s unmapped at %p (mmap)\n", item.fileName, item.memAddress);
                    *M = deleteAtPositionMem(pos, M);
                }
            }
        }
    }
}

void memoryDeallocShared(char* pieces[MAX], int chainWords, tListMem* M){

    if(chainWords == 3){ //no key specified: print list elements allocated with createshared and shared
        printMemList(3, M);
    }
    else{
        tMemKey key;
        tPosM pos;
        tItemM item;

        key = strtoul(pieces[3], NULL, 10);

        pos = findMemKey(key, 3, M);

        if (pos == LNULL){ //key not found: print list elements allocated with createshared and shared
           printMemList(3, M);
        }
        else{
            item = getItemMem(pos, M);
            if(shmdt(item.memAddress) != 0){ //an error occurred
                printf("Error: %s\n", strerror(errno));
            }
            else{
                printf("Shared memory with key %d detached at %p (shared)\n", item.memKey, item.memAddress);
                *M = deleteAtPositionMem(pos, M);
            }
        }
    }
}

void memoryDeallocAddress(char* pieces[MAX], int chainWords, tListMem* M){
    tMemAddress memAddress = NULL;
    tPosM pos = LNULL;
    tItemM item;

    memAddress = (void*) strtoul(pieces[2], NULL, 0);
        //base = 0 makes strtoul to auto detect the number base (hexadecimal)

    pos = findMemAddress(memAddress, M);

    if (pos == LNULL){ //key not found: print list elements allocated with createshared and shared
        printMemList(3, M);
    }
    else{
        item = getItemMem(pos, M);

        if(item.memAllocationType == 1){ //malloc

            free(item.memAddress);
            printf("%ld bytes deallocated at %p (malloc)\n", item.size, item.memAddress);
            *M = deleteAtPositionMem(pos, M);
        }

        if(item.memAllocationType == 2){ //mmap

            if(munmap(item.memAddress, item.size) != 0){ //an error occurred while unmapping the file
                printf("Error: %s\n", strerror(errno));
            }
            else{
                if(close(item.fd) != 0){ //an error occurred while closing the file
                    printf("Error: %s\n", strerror(errno));
                }
                else{
                    printf("File %s unmapped at %p (mmap)\n", item.fileName, item.memAddress);
                    *M = deleteAtPositionMem(pos, M);
                }
            }
        }

        if(item.memAllocationType == 3){ //shared memory

            if(shmdt(item.memAddress) != 0){ //an error occurred
                printf("Error: %s\n", strerror(errno));
            }
            else{
                printf("Shared memory with key %d detached at %p (shared)\n", item.memKey, item.memAddress);
                *M = deleteAtPositionMem(pos, M);
            }
        }
    }
}

void memoryDealloc(char* pieces[MAX], int chainWords, tListMem* M) {

    if (chainWords == 2) { //"memory -dealloc": print all the allocated list elements
        printMemList(0, M);
    }
    else {
        if(!strcmp(pieces[2], "-malloc")){
            memoryDeallocMalloc(pieces, chainWords, M);
        }
        else if(!strcmp(pieces[2], "-mmap")){
            memoryDeallocMmap(pieces, chainWords, M);
        }
        else if(!strcmp(pieces[2], "-shared")){
            memoryDeallocShared(pieces, chainWords, M);
        }
        else{
            memoryDeallocAddress(pieces, chainWords, M);
        }
    }
}


void memoryDeleteKey(char* pieces[MAX], int chainWords){

    if (chainWords == 2) { //no key specified
        printf("Error: please specify a key to delete\n");
    }
    else {
        tItemM item;

        item.memKey = strtoul(pieces[2], NULL, 10);

        int id;
        int flags = 0777;
        if ((id=shmget(item.memKey, 0, flags)) == -1){
            printf("Error: %s\n", strerror(errno));
        }
        else{
            if(shmctl(id,IPC_RMID,NULL) == -1){
                printf("Error: %s\n", strerror(errno));
            }
            else{
                printf("Key %d of shared memory region removed\n", item.memKey);
            }
        }
    }
}


void memoryShow(char* pieces[MAX], int chainWords, tListMem* M) {

    if (chainWords == 2) { //no arguments: print the memory addresses of three program functions, three extern (global)
                            //variables and three automatic (local) variables.

        //local variables definition
        int local1;
        int local2;
        int local3;

        printf("Program functions: %p, %p, %p\n", &readInput, &splitChain, &processInput);
        printf("Global variables: %p, %p, %p\n", &global1, &global2, &global3);
        printf("Local variables: %p, %p, %p\n", &local1, &local2, &local3);

    } else {
        if(!strcmp(pieces[2], "-malloc")){
            printMemList(1, M);
        }
        else if(!strcmp(pieces[2], "-mmap")){
            printMemList(2, M);
        }
        else if(!strcmp(pieces[2], "-shared")){
            printMemList(3, M);
        }
        else if(!strcmp(pieces[2], "-all")){
            printMemList(0, M);
        }
    }
}

void memoryShowVars(){
    //local variables definition
    int local1;
    int local2;
    int local3;

    //global variables defined at the top of this document

    printf("Local variables: %p, %p, %p\n", &local1, &local2, &local3);
    printf("Global variables: %p, %p, %p\n", &global1, &global2, &global3);
}

void memoryShowFuncs(){
    printf("Program functions: %p, %p, %p\n", &readInput, &splitChain, &processInput);
    printf("Library functions: %p, %p, %p\n", &malloc, &strcpy, &free);
}


void memoryDoPmap() {
    FILE *fp;
    char path[MAX];

    //get pid of the current shell
    pid_t pid = getpid();
    char command[MAX];
    sprintf(command, "pmap %d", pid);

    //open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    //read the output a line at a time - output it
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    pclose(fp);
}


void cmdMemory(char* pieces[MAX], int chainWords, tListMem* M){

//    – memory -allocate . . .
//    – memory -dealloc . . .
//    – memory -deletekey cl . . .
//    – memory -show . . .
//    – memory -show-vars
//    – memory -show-funcs
//    – memory -pmap


    if(chainWords == 1){ //"memory": print all the list elements
        printMemList(0, M);
    }
    else{
        if(!strcmp(pieces[1], "-allocate")){
            memoryAllocate(pieces, chainWords, M);
        }
        else if(!strcmp(pieces[1], "-dealloc")){
            memoryDealloc(pieces, chainWords, M);
        }
        else if(!strcmp(pieces[1], "-deletekey")){
            memoryDeleteKey(pieces, chainWords);
        }
        else if(!strcmp(pieces[1], "-show")){
            memoryShow(pieces, chainWords, M);
        }
        else if(!strcmp(pieces[1], "-show-vars")){
            memoryShowVars();
        }
        else if(!strcmp(pieces[1], "-show-funcs")){
            memoryShowFuncs();
        }
        else if(!strcmp(pieces[1], "-dopmap")){
            memoryDoPmap();
        }
    }
}

void cmdMemDump(char* pieces[MAX], int chainWords) {

    int cont = 25;
    int i;
    int n;
    unsigned char c;
    char* memAddress;

    if (chainWords >= 2) { //"memdump addr ..."

        if (chainWords == 3) { //"memdump addr cont": show cont bytes of addr
            cont = (int) strtol(pieces[2], NULL, 10);
        }

        memAddress = (void *) strtoul(pieces[1], NULL, 16);

        for(n = 0; n <= cont; n=n+25){
            //print the characters
            for (i = n; i < MIN(cont, (n+25)); i++) {
                c = *(memAddress + i);
                if (c >= 32 && c < 127) {
                    printf(" %c ", c);
                }
                else{
                    printf("   ");
                }
            }
            printf("\n");

            //print the hex values
            for (i = n; i < MIN(cont, (n+25)); i++) {
                c = *(memAddress + i);
                printf("%02X ", c);
            }
            printf("\n");
        }

    }
}

void cmdMemFill(char* pieces[MAX], int chainWords){

    int cont = 128;
    char c = 'A';
    int i;
    char* memAddress;

    if (chainWords >= 2) { //"memfill addr ..."

        if (chainWords == 3) { //"memfill addr cont": fill cont bytes with 'A'
            cont = (int) strtol(pieces[2], NULL, 10);
        }

        if (chainWords == 4) { //"memfill addr cont byte": fill cont bytes with c
            c = (char) strtol(pieces[3], NULL, 16); //value is given in hexadecimal
        }

        memAddress = (void *) strtoul(pieces[1], NULL, 16);

        for(i = 0; i <= cont; i++){
            *(memAddress + i) = c;
        }

    }

}

void doRecursiva (int n)
{
    char automatico[4096];
    static char estatico[4096];
    printf ("parameter n: %d in %p   ",n,&n);
    printf ("static array in %p   ",estatico);
    printf ("automatic array in %p\n",automatico);
    n--;
    if (n>0)
        doRecursiva(n);
}

void cmdRecurse(char* pieces[MAX], int chainWords){
    int n;

    if(chainWords == 2){
        n = strtol(pieces[1], NULL, 10);

        doRecursiva(n);
    }
}

void cmdReadFile(char* pieces[MAX], int chainWords){
    char filename[MAX];
    char* memAddress;
    size_t cont;
    int fd;
    struct stat state;

    if(chainWords >= 3){
        strcpy(filename, pieces[1]);
        memAddress = (void*) strtoul(pieces[2],NULL,16);

        if((fd = open(filename, O_RDONLY)) == -1){ //error opening the file
            printf("Error: %s\n", strerror(errno));
        }
        else{

            if(fstat(fd, &state) == -1){ //error getting the state of the file
                printf("Error: %s\n", strerror(errno));
            }
            else{

                if(chainWords == 4){ //cont was specified: read only cont bytes
                    cont = strtol(pieces[3], NULL, 10);
                }
                else{ //read complete file
                    cont = state.st_size;
                }

                if(read(fd, memAddress, cont) == -1){
                    printf("Error: %s\n", strerror(errno));
                }
                else{
                    printf("%ld bytes from file %s were read in %p\n", cont, filename, memAddress);
                }

                close(fd);

            }
        }
    }
}

void cmdWriteFile(char* pieces[MAX], int chainWords){
    char filename[MAX];
    char* memAddress;
    int filename_index;
    long cont;
    int flags;
    int fd;

    if(chainWords >= 4){

        if(chainWords == 4 && !strcmp(pieces[1], "-o")){
            return;
        }

        if(chainWords == 5 && !strcmp(pieces[1], "-o")){
            filename_index = 2;
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        }
        else{
            filename_index = 1;
            flags = O_WRONLY | O_CREAT | O_EXCL;
        }

        strcpy(filename, pieces[filename_index]);
        memAddress = (void*) strtoul(pieces[filename_index + 1],NULL,16);
        cont = strtol(pieces[filename_index + 2], NULL, 10);

        if((fd = open(filename, flags, 0665)) == -1){ //error opening the file
            printf("Error: %s\n", strerror(errno));
        }
        else{
            if(write(fd, memAddress, cont) == -1){
                printf("Error: %s\n", strerror(errno));
            }
            else{
                printf("%ld bytes from address %p were written in %s\n", cont, memAddress, filename);
            }

            close(fd);

        }
    }
}

//-----------------------------------------------------------

void cmdGetPriority(char* pieces[MAX], int chainWords){
    pid_t pid;
    int priority;

    if(chainWords == 1){
        pid = getpid();
    }
    else{
        pid = strtol(pieces[1], NULL, 10);
    }

    if((priority = getpriority(PRIO_PROCESS, pid)) == -1){
        printf("Error: %s\n", strerror(errno));
    }
    else{
        printf("Priority of process %d: %d\n", pid, priority);
    }
}

void cmdSetPriority(char* pieces[MAX], int chainWords){
    pid_t pid;
    int priority;

    if(chainWords == 1){ //no arguments given: get priority of the shell
        pid = getpid();

        if((priority = getpriority(PRIO_PROCESS, pid)) == -1){
            printf("Error: %s\n", strerror(errno));
        }
        else{
            printf("Priority of process %d: %d\n", pid, priority);
        }
    }

    else{

        if(chainWords == 2) { //change shell's priority to the value given
            pid = getpid();
            priority = strtol(pieces[1], NULL, 10);
        }

        if(chainWords == 3) { //change process with pid given to the value given
            pid = strtol(pieces[1], NULL, 10);
            priority = strtol(pieces[2], NULL, 10);
        }


        if(setpriority(PRIO_PROCESS, pid, priority) == -1){
            printf("Error: %s\n", strerror(errno));
        }
        else{
            printf("Priority of process %d changed to %d\n", pid, priority);
        }
    }
}

char* NombreUsuario (uid_t uid)
{
    struct passwd *p;
    if ((p=getpwuid(uid))==NULL)
        return NULL;
    return p->pw_name;
}

uid_t UidUsuario (char * nombre)
{
    struct passwd *p;
    if ((p=getpwnam (nombre))==NULL)
        return (uid_t) -1;
    return p->pw_uid;
}

void cmdGetUid(){
    uid_t ruid;
    uid_t euid;
    char* userName = NULL;

    ruid = getuid();
    euid = geteuid();
    if((userName = NombreUsuario(ruid)) == NULL){
        printf("Error: Impossible to get user name\n");
    }
    else{
        printf("Real user ID: %d (%s)\n", ruid, userName);
        printf("Effective user ID: %d (%s)\n", euid, userName);
    }
}

int cmdSetUid(char* pieces[MAX], int chainWords){
    //RETURN VALUE: 0 on success, -1 if an error occurred
    uid_t uid;

    if(chainWords == 1){
        cmdGetUid();
    }
    else{
        if(chainWords == 2){ // "-l" not given: id represents the uid
            uid = strtol(pieces[1], NULL, 10);
        }

        if(chainWords == 3){ // "-l" given: id represents the user name
            if((uid = UidUsuario(pieces[2])) == -1){
                printf("Error: The user does not exist\n");
                return -1;
            }
        }

        if(setuid(uid) == -1){
            printf("Error changing the effective user ID: %s\n", strerror(errno));
            return -1;
        }
        else{
            printf("Effective user ID changed to %d\n", uid);

        }
    }
    return 0;
}

void cmdFork(){
    pid_t pid;

    if((pid = fork()) == -1){
        printf("Error: %s\n", strerror(errno));
    }
    else{

        if(pid != 0){ //parent process
            printf("Executing process %d\n", pid);
            waitpid(pid, NULL, 0);
        }
    }
}

void cmdExecute(char* pieces[MAX], int chainWords){

    char* arguments[MAX] = {NULL};
        //NOTE: the execvp() library function requires the program filename and an array
        //whose first argument is the program filename, followed by the arguments

    int argNo = 0;

    if(chainWords == 1){ //no program to execute specified
        printf("Error: no program specified\n");
        quit = true;
    }
    else{

        //check the program and the arguments specified
        for(int i = 1; i < chainWords; i++){
            if(pieces[i][0] == '@'){ //check if "@pri" is specified
                int priority;

                priority = strtol(&pieces[i][1], NULL, 10);

                if(setpriority(PRIO_PROCESS, getpid(), priority) == -1){
                    printf("Error: %s\n", strerror(errno));
                }
                else{
                    printf("Priority of process %d changed to %d\n", getpid(), priority);
                }

            }

            else{ //add the arguments to the new array
                arguments[argNo] = pieces[i];
                argNo++;
            }
        }


        if(execvp(pieces[1], arguments) == -1){
            printf("Error: %s\n", strerror(errno));
            quit = true;
        }

    }
}

void cmdForeground(char* pieces[MAX], int chainWords){
    pid_t pid;

    if((pid = fork()) == -1){
        printf("Error: %s\n", strerror(errno));
    }
    else{

        if(pid == 0){ //child process
            cmdExecute(pieces, chainWords);
        }

        waitpid(pid, NULL, 0); //parent process
    }
}

void cmdBackground(char* pieces[MAX], int chainWords, tListProc* P){
    pid_t pid;
    tItemP newProcEntry;


    if((pid = fork()) == -1){
        printf("Error: %s\n", strerror(errno));
    }
    else{

        //NOTE: fork() returns 0 to the new process and the new process ID
        //to the old process

        if(pid == 0){ //child process
            cmdExecute(pieces, chainWords);
        }
        else{ //parent process

            //new entry creation for the child process ****************************

            //pid
            newProcEntry.pid = pid;

            //process priority
            if((newProcEntry.priority = getpriority(PRIO_PROCESS, pid)) == -1){
                printf("Error getting the priority: %s\n", strerror(errno));
            }

            //command line executed
            newProcEntry.command = malloc(sizeof(char *) * (chainWords - 1));
            newProcEntry.commandWords = 0;

            for(int i = 1; i < chainWords; i++){
                newProcEntry.command[i-1] = calloc(strlen(pieces[i]) + 1, sizeof(char));
                strncpy(newProcEntry.command[i-1], pieces[i], strlen(pieces[i]));
                newProcEntry.commandWords++;
            }

            //time
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            newProcEntry.time = tm;

            //process state, signal and return value initialization
			newProcEntry.state = 0;
            newProcEntry.returnValue = malloc(sizeof(int));
            newProcEntry.signal = malloc(sizeof(int));


            insertItemProc(newProcEntry, PNULL, P);


            //process status update
            updateProcessState(pid, true, P);
        }
    }
}

void cmdRunAs(char* pieces[MAX], int chainWords, tListProc* P){

    char* setUidPieces[MAX] = {NULL};

    setUidPieces[0] = "setuid";
    setUidPieces[1] = "-l";
    setUidPieces[2] = pieces[1]; //login username



    if(cmdSetUid(setUidPieces, 3) == 0){

        char* programPieces[MAX] = {NULL};
        int programWords = 1;
        bool backgroundOption = false;

        for (int i = 2; i < chainWords; i++){
            if(pieces[i][0] == '&'){
                backgroundOption = true;
            }
            else{
                programPieces[i-1] = pieces[i];
                programWords++;
            }
        }

        if (backgroundOption){
            cmdBackground(programPieces, programWords, P);
        }
        else{
            cmdForeground(programPieces, programWords);
        }
    }
}

void cmdExecuteAs(char* pieces[MAX], int chainWords, tListProc* P){

    char* setUidPieces[MAX] = {NULL};

    setUidPieces[0] = "setuid";
    setUidPieces[1] = "-l";
    setUidPieces[2] = pieces[1]; //login username

    if(cmdSetUid(setUidPieces, 3) == 0){

        char* programPieces[MAX];
        int programWords = 1;

        for (int i = 2; i < chainWords; i++){
            programPieces[i-1] = pieces[i];
            programWords++;
        }

        cmdExecute(programPieces, programWords);
    }
}

void cmdOther(char* pieces[MAX], int chainWords, tListProc* P){

    char* programPieces[MAX] = {NULL};
    int programWords = 1;
    bool backgroundOption = false;

    for(int i = 0; i < chainWords; i++){

        if(pieces[i][0] == '&'){
            backgroundOption = true;
        }
        else{
            programPieces[i+1] = pieces[i];
            programWords++;
        }
    }

    if(backgroundOption){
        cmdBackground(programPieces, programWords, P);
    }
    else{
        cmdForeground(programPieces, programWords);
    }

}

void cmdProc(char* pieces[MAX], int chainWords, tListProc* P){

    if(chainWords == 1){ //no arguments specified
        printAllProcesses(P);
    }
    else if(chainWords == 2){
        if(strcmp(pieces[1], "-fg") == 0){ //no pid specified
            printAllProcesses(P);
        }
        else{
            pid_t pid = strtol(pieces[1], NULL, 10);
            tPosP p = findItemProc(pid, *P);

            if(p == PNULL){ //process with specified PID is not in the list
                printAllProcesses(P);
            }
            else{
                printProcess(pid, P);
            }
        }
    }
    else if(chainWords == 3){ // "-fg" and pid specified
        pid_t pid = strtol(pieces[2], NULL, 10);
        tPosP p = findItemProc(pid, *P);

        if(p == PNULL){ //process with specified PID is not in the list
            printAllProcesses(P);
        }
        else{
            updateProcessState(pid, false, P);
            printProcess(pid, P);
            deleteAtPositionProc(p, P);
        }
    }
}

void cmdDeleteProcs(char* pieces[MAX], int chainWords, tListProc* P){

    if(chainWords == 1){ //no arguments given: print list of processes
        printAllProcesses(P);
    }
    else if (chainWords == 2){

        if(!isEmptyListProc(*P)){
            tPosP p = firstProc(*P);

            while (p != PNULL){
                tItemP process = getItemProc(p, *P);

                if(strcmp(pieces[1], "-term") == 0){
                    if(process.state == TERMNORMALLY){
                        deleteAtPositionProc(p, P);
                    }
                }

                if(strcmp(pieces[1], "-sig") == 0){
                    if(getItemProc(p, *P).state == TERMSIGNAL){
                        deleteAtPositionProc(p, P);
                    }
                }

                p = nextProc(p, *P);
            }
        }
    }
}



//-----------------------------------------------------------

//INPUT PROCESSING FUNCTIONS --------------------------------

void readInput(char* chain, tList* L) {
    tItemL cmdData;
    fgets(chain,MAX,stdin);
    strcpy(cmdData.name, chain);
    if(!insertItem(cmdData, LNULL, L))
        printf("Error while inserting the current command in the historic list\n");

}

int splitChain(char* chain, char* pieces[]) { //devuelve el número de palabras introducidas
    int i = 1;
    if ((pieces[0] = strtok(chain, " \n\t")) == NULL) //strtok coge la cadena y la trocea si encuentra \n o \t
        return 0;
    while ((pieces[i] = strtok(NULL, " \n\t")) != NULL) //las siguientes llamadas a strtok pasan NULL como primer argumento
        i++;
    return i;
}

void processInput(char* chain, int* repeatCounter, tList* L, tListMem* M, tListProc* P) {
    char* pieces[MAX] = {NULL};
    int chainWords = 0;

    chainWords = splitChain(chain, pieces);
    //chainWords: no. of words typed by the user

    if (chainWords == 0) //no se han introducido palabras
        return;


    if (!strcmp(pieces[0], "authors"))
        cmdAuthors(pieces[1]);

    else if (!strcmp(pieces[0], "getpid"))
        cmdGetPid(pieces[1]);

    else if(!strcmp(pieces[0], "getppid"))
        cmdGetPPid(pieces[1]);

    else if (!strcmp(pieces[0], "pwd"))
        cmdPwd(pieces[1]);

    else if (!strcmp(pieces[0], "chdir"))
        cmdChdir(pieces[1]);

    else if(!strcmp(pieces[0], "date"))
        cmdDate(pieces[1]);

    else if(!strcmp(pieces[0], "time"))
        cmdTime(pieces[1]);

    else if (!strcmp(pieces[0], "historic"))
        cmdHistoric(pieces[1], chain, repeatCounter, L, M, P);

    else if(!strcmp(pieces[0], "quit") || !strcmp(pieces[0], "end") || !strcmp(pieces[0], "exit"))
        quit = true;



    else if(!strcmp(pieces[0], "create"))
        cmdCreate(pieces[1], pieces[2], chainWords);

    else if(!strcmp(pieces[0], "delete"))
        cmdDelete(pieces, chainWords);

    else if(!strcmp(pieces[0], "list"))
        cmdList(pieces, chainWords);



    else if(!strcmp(pieces[0], "memory"))
        cmdMemory(pieces, chainWords, M);

    else if(!strcmp(pieces[0], "memdump")){
        cmdMemDump(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "memfill")){
        cmdMemFill(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "recurse")){
        cmdRecurse(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "readfile")){
        cmdReadFile(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "writefile")){
        cmdWriteFile(pieces, chainWords);
    }



    else if(!strcmp(pieces[0], "getpriority")){
        cmdGetPriority(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "setpriority")){
        cmdSetPriority(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "getuid")){
        cmdGetUid();
    }

    else if(!strcmp(pieces[0], "setuid")){
        cmdSetUid(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "fork")){
        cmdFork();
    }

    else if(!strcmp(pieces[0], "execute")){
        cmdExecute(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "foreground")){
        cmdForeground(pieces, chainWords);
    }

    else if(!strcmp(pieces[0], "background")){
        cmdBackground(pieces, chainWords, P);
    }

    else if(!strcmp(pieces[0], "run-as")){
        cmdRunAs(pieces, chainWords, P);
    }

    else if(!strcmp(pieces[0], "execute-as")){
        cmdExecuteAs(pieces, chainWords, P);
    }

    else if(!strcmp(pieces[0], "listprocs")){
        printAllProcesses(P);
    }

    else if(!strcmp(pieces[0], "proc")){
        cmdProc(pieces, chainWords, P);
    }

    else if(!strcmp(pieces[0], "deleteprocs")){
        cmdDeleteProcs(pieces, chainWords, P);
    }


    else cmdOther(pieces, chainWords, P);
}



int main() {
    char chain[MAX];
    int repeatCounter = 0;
    static tList L;
    static tListMem M;
    static tListProc P;
    createEmptyList(&L);
    createEmptyListMem(&M);
    createEmptyListProc(&P);

    while(1){
        printf("> ");
        readInput(chain, &L);
        processInput(chain, &repeatCounter, &L, &M, &P);
        repeatCounter = 0;
        if(quit == true)
            break;
    }

    deleteList(&L);
    deleteListMem(&M);
    deleteListProc(&P);

    return 0;
}
