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

//FUNCTIONS DECLARATION ------------------------------------------------------

void print_perms(mode_t st);
void print_long_format(struct dirent* file, struct stat state, char* currentFilePath, bool calledByListName);
void listDir(char* pathname, bool longOption, bool hidOption);
void listDirRec(char* pathname, bool longOption, bool hidOption);
void listName(char* name, bool longOption);
void cmdList(char* pieces[MAX], int chainWords);

//-----------------------------------------------------------

//CMD FUNCTIONS ---------------------------------------------

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

//MAIN FUNCTION ---------------------------------------------

int main(int argc, char *argv[]) {
    cmdList(argv, argc);
    return 0;
}
