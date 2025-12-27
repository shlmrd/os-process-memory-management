/**
 * Usage : ./nachos -rs 1234 -x ../test/lockthreads_putchar
 * 
 * Intérêt :
 * Tester le mécanisme de création et d'exécution des threads utilisateurs.
 * dans Nachos avec plusieurs threads en parallèle avec putchar
 */

#include "syscall.h"

void fc (void *nom) {
    char *x=(char *)nom;
    volatile int i;
    for (i = 0; i < 10; i++) {
        PutChar(*x);      //affiche le caractère pointé par x
    }
    ThreadExit();         //quitte ce thread
}

int main () {
    char a='a', b='b', c='c';
    ThreadCreate(fc, &a);
    ThreadCreate(fc, &b);
    ThreadCreate(fc, &c);
    ThreadExit();
    return 0;
}

