// Cristian Ferreiro Montoiro - cristian.ferreiro - 49669053R
// Xoel González Pereira - xoel.gonzalezp - 45163741K


#ifndef P2_LIST_H
#define P2_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX 1024


//MEMORY LIST TYPES ---------------------------------------------------------------------

#define LNULL (-1)
#define LMAX 4096

/*
 * store:
 * - memory address of the block
 * - size of the block
 * - time when it was allocated
 * - type of allocation: malloc, shared memory or mapped file
 * - other info:
 *      - name of file for mapped files
 *      - key for shared memory
 *      - ...
 */

typedef void* tMemAddress;
typedef size_t tSize;
typedef int tMemAllocationType; //1 = malloc; 2 = mapped file; 3 = shared memory
typedef char tFileName[MAX]; //-mmap only
typedef int tFileDescriptor; //-mmap only
typedef int tMemKey; //-createshared and -shared only
typedef struct tm tTime;

typedef int tPosM;

typedef struct {
    tMemAddress memAddress;
    tSize size;
    tMemAllocationType memAllocationType;
    tFileName fileName;
    tFileDescriptor fd;
    tMemKey memKey;
    tTime time;

} tItemM;

typedef struct {
    tItemM data[LMAX];
    tPosM lastPos;
} tListMem;


//MEMORY LIST FUNCTIONS DECLARATION ----------------------------------------------------

void createEmptyListMem(tListMem* L);
tPosM nextMem(tPosM p, tListMem* L);
tPosM firstMem(tListMem* L);
tPosM lastMem(tListMem* L);
bool insertItemMem(tItemM d, tPosM p, tListMem* L);
tItemM getItemMem(tPosM p, tListMem* L);
tListMem deleteAtPositionMem(tPosM p, tListMem* L);
void deleteListMem(tListMem* L);

tPosM findMemAddress(tMemAddress memAddress, tListMem* M);
tPosM findSize(tSize size, tMemAllocationType memAllocationType, tListMem* M);
tPosM findFileName(tFileName fileName, tMemAllocationType memAllocationType, tListMem* M);
tPosM findMemKey(tMemKey key, tMemAllocationType memAllocationType, tListMem* M);

void printMemList(int option, tListMem* L);
void printMemItem(tItemM item);



#endif //P2_LIST_H
