// Cristian Ferreiro Montoiro - cristian.ferreiro - 49669053R
// Xoel González Pereira - xoel.gonzalezp - 45163741K

#include "bgproclist.h"




//LINKED LIST FUNCTIONS -------------------------------------------------------

tListProc createEmptyListProc(tListProc* L)
{
    *L = PNULL;
    return (*L);
}

bool createNode(tPosP* p) //internal function
{
    *p = malloc(sizeof(struct tNode));
    return (*p != PNULL); /* if there is memory returns true */
}

bool isEmptyListProc(tListProc L) //checks if the list is empty
{
    return (L == PNULL);
}

tPosP firstProc(tListProc L)
{
    return L; /* L is a pointer to the first element of the list */
}

tPosP lastProc(tListProc L)
{
    tPosP p;
    for(p = L; p->next != PNULL; p = p->next)
        ; /* moves p until the last position to p is null */
    return p;
}

tPosP nextProc(tPosP p, tListProc L)
{
    if (p->next == PNULL) /* p is the last position already */
        return PNULL;
    else
        return (p->next);
}


bool insertItemProc(tItemP d, tPosP p, tListProc* L)
{
    tPosP q, r;
    if(!createNode(&q))
        return false; //if there is no memory
    else
    {
        q->data = d;
        q->next = PNULL;

        if (*L == PNULL) //L is empty
            *L = q; //L is a pointer to the first node

        else // L is not empty
        {
            if (p == PNULL) //the given position is NULL: insert at the end
            {
                for (r = *L; r->next != PNULL; r = r->next)
                    ; //we move r until the last node created
                r->next = q;
            }
            else //the given position is not NULL
            {
                if (p == *L) //insert at beginning
                {
                    q->next = p;
                    *L = q;
                }
                else //insert in the position
                {
                    q->data = p->data;
                    p->data = d;
                    q->next = p->next;
                    p->next = q;
                }
            }
        }
        return true;
    }
}

tPosP findItemProc(pid_t pid, tListProc L)
{
    tPosP p;
    for(p = firstProc(L); ((p != PNULL) && (p->data.pid != pid)); p = p->next)
        ;
    if((p != PNULL) && (p->data.pid == pid))
        return p;
    else
        return PNULL;
}

tListProc deleteAtPositionProc(tPosP p, tListProc* L)
{
    tItemP process = getItemProc(p, *L);
    for(int i = 0; i < process.commandWords; i++){
        free(process.command[i]);
    }
    free(process.command);
    free(process.signal);
    free(process.returnValue);

    tPosP q;
    if (p == *L) //p points to the first node
    {
        *L = p->next;
        free(p);
    }
    else
    if(p->next == PNULL) //p points to the last node
    {
        for(q=*L; q->next != p; q=q->next)
            ; //moves q until the next position to q is p
        q->next = PNULL;
        free(p);
    }
    else //general case
    {
        q = p->next; //q points to the next node to p
        p->data = q->data; //copy the data in q to p
        p->next = q->next; //the next node to p is the next node to q
        free(q);
    }
    return (*L);
}

tItemP getItemProc(tPosP p, tListProc L)
{
    return (p->data);
}

void deleteListProc(tListProc* L)
{
    tPosP p;
    tItemP process;

    while(*L != PNULL)
    {
        p = *L;

        process = getItemProc(p, *L);
        for(int i = 0; i < process.commandWords; i++){
            free(process.command[i]);
        }
        free(process.command);
        free(process.signal);
        free(process.returnValue);

        *L = (*L)->next;
        free(p);
    }
}




void updateProcessState(pid_t pid, bool flags, tListProc* L){
    //this function updates the state of a process in the background processes list

    tPosP p = findItemProc(pid, *L);

    if(p != PNULL){ //the process with the specified PID is not in the specified list

        int wstatus;

        if(pid == waitpid(pid, &wstatus, (flags == false) ? 0 : WNOHANG | WUNTRACED | WCONTINUED)){
            //wstatus contains valid information: process state has changed

            tItemP process = getItemProc(p, *L);

            if (WIFCONTINUED(wstatus)) {
                process.state = RUNNING; }
            if (WIFSTOPPED(wstatus)) {
                process.state = STOPPED; *process.signal = WTERMSIG(wstatus); }
            if (WIFEXITED(wstatus)) {
                process.state = TERMNORMALLY; *process.returnValue = WEXITSTATUS(wstatus); }
            if (WIFSIGNALED(wstatus)) {
                process.state = TERMSIGNAL; *process.signal = WTERMSIG(wstatus); }

            p->data = process;
        }
    }
}


int Senal(char * sen) /*devuelve el numero de senal a partir del nombre*/
{
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal;
    return -1;
}

char* NombreSenal(int sen) /*devuelve el nombre de senal a partir de la senal*/
{
/*para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}



const char* strProcState(enum tProcState state){
    switch (state) {
        case 0: return "Running";
        case 1: return "Stopped";
        case 2: return "Terminated normally";
        case 3: return "Terminated by signal";
        default: return "Unknown state";
    }
}


void printProcess(pid_t pid, tListProc* L){

    if(!isEmptyListProc(*L)){
        tPosP p = findItemProc(pid, *L);

        if (p != PNULL){

            updateProcessState(pid, true, L);
            tItemP process = getItemProc(p, *L);

            //Time
            char datestring[256];
            strftime(datestring, sizeof(datestring), "%a %b %d %H:%M:%S %Y", &process.time);


            printf("%d  p=%d  %s  %s ", process.pid, process.priority, datestring,
                   strProcState(process.state));


            if(process.state == TERMNORMALLY){
                printf("(Return value: %d)", *process.returnValue);
            }
            if(process.state == STOPPED || process.state == TERMSIGNAL){
                printf("(Signal: %s)", NombreSenal(*process.signal));
            }

            printf("  ");

            for(int j = 0; j < process.commandWords; j++){
                printf("%s ", process.command[j]);
            }

            printf("\n");
        }
    }
}


void printAllProcesses(tListProc* L){

    if(!isEmptyListProc(*L)){

        tPosP p = firstProc(*L);
        while (p != PNULL){

            tItemP process = getItemProc(p, *L);
            updateProcessState(process.pid, true, L);
            process = getItemProc(p, *L);

            //Time
            char datestring[256];
            strftime(datestring, sizeof(datestring), "%a %b %d %H:%M:%S %Y", &process.time);


            printf("%d  p=%d  %s  %s ", process.pid, process.priority, datestring,
                   strProcState(process.state));


            if(process.state == TERMNORMALLY){
                printf("(Return value: %d)", *process.returnValue);
            }
            if(process.state == STOPPED || process.state == TERMSIGNAL){
                printf("(Signal: %s)", NombreSenal(*process.signal));
            }

            printf("  ");

            for(int i = 0; i < process.commandWords; i++){
                printf("%s ", process.command[i]);
            }

            printf("\n");


            p = nextProc(p, *L);
        }
    }
}


