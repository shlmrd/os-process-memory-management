#include "syscall.h"

void worker(void *p) {
    char c = *(char*)p;
    int i;
    for (i = 0; i < 10; i++) {
        PutChar(c);
    }
    ThreadExit();
}

int main() {
    /* Avec UserStacksAreaSize = 4096 -> 16 piles -> 15 threads max */
    static char letters[] = "abcdefghijklmno";  /* 15 threads */
    int i;

    for (i = 0; letters[i] != '\0'; i++) {
        ThreadCreate(worker, &letters[i]);
    }

    ThreadExit();
    return 0;
}

