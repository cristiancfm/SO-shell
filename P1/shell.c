// Cristian Ferreiro Montoiro - cristian.ferreiro
// Xoel González Pereira - xoel.gonzalezp

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#define MAX 1024

//TYPES ---------------------------------------------------------------------
#define LNULL -1
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
void cmdHistoric(char* argument, char* chain, int* repeatCounter, bool* leave, tList* L);


void cmdCreate(char* argument1, char* argument2, int chainWordsCount);

void deleteDirRec(char* pathname);
void cmdDelete(char* pieces[MAX], int chainWords);

void print_perms(mode_t st);
void print_long_format(struct dirent* file, struct stat state, char* currentFilePath, bool calledByListName);
void listDir(char* pathname, bool longOption, bool hidOption);
void listDirRec(char* pathname, bool longOption, bool hidOption);
void listName(char* name, bool longOption);
void cmdList(char* pieces[MAX], int chainWords);

void readInput(char* chain, tList* L);
int splitChain(char* chain, char* pieces[]);
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
        strcpy(chain, cmdData.name);

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

    }
    else{
        printf("Error deleting %s: %s\n", pathname, strerror(errno));
    }

}

void cmdDelete(char* pieces[MAX], int chainWords) {

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

            if (opendir(pieces[i]) != NULL) { //es un directorio

                if (recOption) { //borrar archivos y directorios con sus contenidos especificados
                    deleteDirRec(pieces[i]);
                }
                else { //borrar archivos y directorios vacíos especificados
                    if (remove(pieces[i]) != 0) {
                        printf("Error: %s\n", strerror(errno));
                    }
                }
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
    perms[1] = (st && S_IRUSR) ? 'r':'-';
    perms[2] = (st && S_IWUSR) ? 'w':'-';
    perms[3] = (st && S_IXUSR) ? 'x':'-';
    perms[4] = (st && S_IRGRP) ? 'r':'-';
    perms[5] = (st && S_IWGRP) ? 'w':'-';
    perms[6] = (st && S_IXGRP) ? 'x':'-';
    perms[7] = (st && S_IROTH) ? 'r':'-';
    perms[8] = (st && S_IWOTH) ? 'w':'-';
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

//INPUT PROCESSING FUNCTIONS --------------------------------

void readInput(char* chain, tList* L) {
    tItemL cmdData;
    fgets(chain,MAX,stdin);
    strcpy(cmdData.name, chain);
    if(!insertItem(cmdData, LNULL, L))
        printf("Error while inserting the current command in the historic list");

}

int splitChain(char* chain, char* pieces[]) { //devuelve el número de palabras introducidas
    int i = 1;
    if ((pieces[0] = strtok(chain, " \n\t")) == NULL) //strtok coge la cadena y la trocea si encuentra \n o \t
        return 0;
    while ((pieces[i] = strtok(NULL, " \n\t")) != NULL) //las siguientes llamadas a strtok pasan NULL como primer argumento
        i++;
    return i;
}

void processInput(char* chain, bool* leave, int* repeatCounter, tList* L) {
    char* pieces[MAX];
    int chainWords;

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
        cmdHistoric(pieces[1], chain, repeatCounter, leave, L);

    else if(!strcmp(pieces[0], "quit") || !strcmp(pieces[0], "end") || !strcmp(pieces[0], "exit"))
        *leave = true;



    else if(!strcmp(pieces[0], "create"))
        cmdCreate(pieces[1], pieces[2], chainWords);

    else if(!strcmp(pieces[0], "delete"))
        cmdDelete(pieces, chainWords);

    else if(!strcmp(pieces[0], "list"))
        cmdList(pieces, chainWords);



    else printf("%s: not found\n", pieces[0]);
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
