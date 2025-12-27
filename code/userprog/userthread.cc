#ifdef CHANGED

#include "userthread.h"
#include "system.h" 

struct ThreadArgs {
    int f;
    int arg;
    int slot;
}; 

static void StartUserThread(void *schmurtz) {
    ThreadArgs *args = (ThreadArgs *) schmurtz;
    int f = args->f;
    int arg = args->arg;
    int slot = args->slot; //numero de pile 
    delete args;
    currentThread->slot = slot;
    // Initialisation registres MIPS
    currentThread->space->InitRegisters();

    // nouv pile pour ce thread
    int newSP = currentThread->space->AllocateUserStack(slot);
    machine->WriteRegister(StackReg, newSP);
    machine->WriteRegister(PCReg, f);
    machine->WriteRegister(NextPCReg, f + 4);

    // arg dans le registre 4
    machine->WriteRegister(4, arg);
    DEBUG('x', "Thread utilisateur lancé : f=%d arg=%d SP=%d\n", f, arg, newSP);
    machine->DumpMem("threads.svg");
    machine->Run();
    ASSERT(false);
}

int do_ThreadCreate(int f, int arg) {
    Thread *newThread = new Thread("UserThread");
    if (!newThread) return -1;

    newThread->space = currentThread->space;
    int slot = newThread->space->AddThread();
    newThread->slot = slot;
     if (slot == -1) {
        DEBUG('s', "aucune pile libre disponible !\n");
        delete newThread;
        return -1;
    }
    DEBUG('s', "after adding Number of threads %d\n", slot);
    
    ThreadArgs *schmurtz = new ThreadArgs;
    schmurtz->f = f;
    schmurtz->arg = arg;
    schmurtz->slot = slot;
    newThread->Start(StartUserThread, (void*)schmurtz);

    return 0;
}

void do_ThreadExit(void) {
    DEBUG('z', "threadexit %d\n", currentThread->slot);
    int left = currentThread->space->RemoveThread(currentThread->slot);
    DEBUG('s', "after removing Number of threads %d\n", left);
    if (left == 0) { //si il n'a plus de threads
        processCount--;
        if (processCount == 0){
            interrupt->Powerdown();    //on termine Nachos
        }
    }
    currentThread->Finish();
}

static void StartUserProcess(void *arg) {
    Thread *t = (Thread *)arg;
    DEBUG('z', "running thread %p\n", t);
    
    t->space->InitRegisters();
    t->space->RestoreState();

    machine->Run();
    ASSERT(false);

}

int do_ForkExec(const char *s) {
    DEBUG('z', "filename %s\n", s);
    OpenFile *executable = fileSystem->Open(s);
    AddrSpace *space = new AddrSpace(executable);
    Thread *t = new Thread("user process");
    t->space = space;
    t->slot = 0;
    DEBUG('z', "creating thread %p with slot %d\n", t, t->slot);
    processCount++;
    t->Start(StartUserProcess, t);
    return 0;
}

#endif // CHANGED
