// Cristian Ferreiro Montoiro - cristian.ferreiro - 49669053R
// Xoel González Pereira - xoel.gonzalezp - 45163741K


#include "list.h"


//STATIC LIST FUNCTIONS -------------------------------------------------------

void createEmptyListMem(tListMem* L)
{
    L->lastPos = LNULL;
}

tPosM nextMem(tPosM p, tListMem* L)
{
    if(p==L->lastPos)
        return LNULL;
    else
        return (p+1);
}

tPosM firstMem(tListMem* L)
{
    if (L->lastPos != LNULL)
        return 0;
    else
        return LNULL;
}

tPosM lastMem(tListMem* L)
{
    if (L->lastPos != LNULL)
        return L->lastPos;
    else
        return LNULL;
}

bool insertItemMem(tItemM d, tPosM p, tListMem* L)
{
    tPosM i;
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

tItemM getItemMem(tPosM p, tListMem* L)
{
    return (L->data[p]);
}

tListMem deleteAtPositionMem(tPosM p, tListMem* L)
{
    tPosM i;
    L->lastPos--;
    for(i = p; i <= L->lastPos; i++)
    {
        L->data[i] = L->data[i+1];
    }
    return *L;
}

void deleteListMem(tListMem* L)
{
    L->lastPos = LNULL;
}

tPosM findMemAddress(tMemAddress memAddress, tListMem* M)
{
    tPosM p;
    if (M->lastPos == LNULL)
        return LNULL;
    else
    {
        for (p = 0; (p < M->lastPos) && (M->data[p].memAddress != memAddress); p++)
            ;
        {
            if (M->data[p].memAddress == memAddress){
                return p;
            }

            return LNULL;
        }
    }
}

tPosM findSize(tSize size, tMemAllocationType memAllocationType, tListMem* M)
{
    tPosM p;
    if (M->lastPos == LNULL)
        return LNULL;
    else
    {
        for (p = 0; (p <= M->lastPos) && (M->data[p].size != size); p++)
            ;
        {
            if (M->data[p].size == size){
                if(M->data[p].memAllocationType == memAllocationType) {
                    return p;
                }
            }

            return LNULL;
        }
    }
}


tPosM findFileName(tFileName fileName, tMemAllocationType memAllocationType, tListMem* M)
{
    tPosM p;
    if (M->lastPos == LNULL)
        return LNULL;
    else
    {
        for (p = 0; (p <= M->lastPos) && (strcmp(M->data->fileName, fileName) != 0); p++)
            ;
        {
            if (strcmp(M->data->fileName, fileName) == 0){
                if(M->data[p].memAllocationType == memAllocationType) {
                    return p;
                }
            }

            return LNULL;
        }
    }
}


tPosM findMemKey(tMemKey key, tMemAllocationType memAllocationType, tListMem* M)
{
    tPosM p;
    if (M->lastPos == LNULL)
        return LNULL;
    else
    {
        for (p = 0; (p < M->lastPos) && (M->data[p].memKey != key); p++)
            ;
        {
            if (M->data[p].memKey == key){
                if(M->data[p].memAllocationType == memAllocationType) {
                    return p;
                }
            }

            return LNULL;
        }
    }
}

void printMemList(int option, tListMem* L){
    tPosM i;
    tItemM item;

    for(i = firstMem(L); i != LNULL; i = nextMem(i, L)){

        item = getItemMem(i, L);

        if(option == 0){ //print all list
            printMemItem(item);
        }
        if(option == 1){ //print elements allocated with malloc
            if(item.memAllocationType == 1){
                printMemItem(item);
            }
        }
        if(option == 2){ //print elements allocated with mmap
            if(item.memAllocationType == 2){
                printMemItem(item);
            }
        }
        if(option == 3){ //print elements allocated with createshared or shared
            if(item.memAllocationType == 3){
                printMemItem(item);
            }
        }
    }
}

void printMemItem(tItemM item){

    //Time
    char datestring[256];
    strftime(datestring, sizeof(datestring), "%a %b %d %H:%M:%S %Y", &item.time);

    if(item.memAllocationType == 1){
        printf("%p  size:%ld  malloc  %s\n", item.memAddress, item.size, datestring);
    }
    if(item.memAllocationType == 2){
        printf("%p  size:%ld  mmap  %s  (fd:%d)  %s\n", item.memAddress, item.size, item.fileName, item.fd,
               datestring);
    }
    if(item.memAllocationType == 3){
        printf("%p  size:%ld  shared memory  (key %d)  %s\n", item.memAddress, item.size, item.memKey,
               datestring);
    }

}