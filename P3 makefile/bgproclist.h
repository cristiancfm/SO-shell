// Cristian Ferreiro Montoiro - cristian.ferreiro - 49669053R
// Xoel González Pereira - xoel.gonzalezp - 45163741K

#ifndef P2_BGPROCLIST_H
#define P2_BGPROCLIST_H

#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "stdio.h"

#define MAX 1024
#define PNULL NULL

//BACKGROUND PROCESSES LIST TYPES ---------------------------------------------------------

/*
 * store:
 * - process pid
 * - process priority
 * - time when process started
 * - process state (running, stopped, terminated normally or terminated by signal)
 *      - for processes that terminated normally, value returned;
 *      - for processes stopped or terminated by a signal, the name of the signal
 * - command line that process is executing
 */

typedef struct tm tTime;

typedef enum tProcState{ //0 = running; 1 = stopped; 2 = terminated normally; 3 = terminated by signal
    RUNNING = 0,
    STOPPED = 1,
    TERMNORMALLY = 2,
    TERMSIGNAL = 3
} tProcState;


typedef struct {
    pid_t pid;
    int priority;
    char** command;
    int commandWords;
    tTime time;
    tProcState state;
    int* returnValue;
    int* signal;

} tItemP;


typedef struct tNode* tPosP;
struct tNode{
    tItemP data;
    tPosP next;
};

typedef tPosP tListProc;


/******************************SENALES ******************************************/
struct SEN{
    char *nombre;
    int senal;
};
static struct SEN sigstrnum[]={
        "HUP", SIGHUP,
        "INT", SIGINT,
        "QUIT", SIGQUIT,
        "ILL", SIGILL,
        "TRAP", SIGTRAP,
        "ABRT", SIGABRT,
        "IOT", SIGIOT,
        "BUS", SIGBUS,
        "FPE", SIGFPE,
        "KILL", SIGKILL,
        "USR1", SIGUSR1,
        "SEGV", SIGSEGV,
        "USR2", SIGUSR2,
        "PIPE", SIGPIPE,
        "ALRM", SIGALRM,
        "TERM", SIGTERM,
        "CHLD", SIGCHLD,
        "CONT", SIGCONT,
        "STOP", SIGSTOP,
        "TSTP", SIGTSTP,
        "TTIN", SIGTTIN,
        "TTOU", SIGTTOU,
        "URG", SIGURG,
        "XCPU", SIGXCPU,
        "XFSZ", SIGXFSZ,
        "VTALRM", SIGVTALRM,
        "PROF", SIGPROF,
        "WINCH", SIGWINCH,
        "IO", SIGIO,
        "SYS", SIGSYS,
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
        "POLL", SIGPOLL,
#endif
#ifdef SIGPWR
        "PWR", SIGPWR,
#endif
#ifdef SIGEMT
        "EMT", SIGEMT,
#endif
#ifdef SIGINFO
        "INFO", SIGINFO,
#endif
#ifdef SIGSTKFLT
        "STKFLT", SIGSTKFLT,
#endif
#ifdef SIGCLD
        "CLD", SIGCLD,
#endif
#ifdef SIGLOST
        "LOST", SIGLOST,
#endif
#ifdef SIGCANCEL
        "CANCEL", SIGCANCEL,
#endif
#ifdef SIGTHAW
        "THAW", SIGTHAW,
#endif
#ifdef SIGFREEZE
        "FREEZE", SIGFREEZE,
#endif
#ifdef SIGLWP
        "LWP", SIGLWP,
#endif
#ifdef SIGWAITING
        "WAITING", SIGWAITING,
#endif
        NULL,-1,

};

//BACKGROUND PROCESSES LIST FUNCTIONS DECLARATION ------------------------------------------

tListProc createEmptyListProc(tListProc * L);
bool isEmptyListProc(tListProc L);
tPosP firstProc(tListProc L);
tPosP lastProc(tListProc L);
tPosP nextProc(tPosP p, tListProc L);
bool insertItemProc(tItemP d, tPosP p, tListProc* L);
tListProc deleteAtPositionProc(tPosP p, tListProc* L);
tItemP getItemProc(tPosP p, tListProc L);
tPosP findItemProc(pid_t pid, tListProc L);
void deleteListProc(tListProc* L);

void updateProcessState(pid_t pid, bool flags, tListProc* L);
int Senal(char * sen);
char* NombreSenal(int sen);

const char* strProcState(enum tProcState state);
void printProcess(pid_t pid, tListProc* L);
void printAllProcesses(tListProc* L);

#endif //P2_BGPROCLIST_H
