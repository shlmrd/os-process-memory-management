/**
 * Usage : ./nachos -rs 1234 -x ../test/lockthreads_getchar
 * 
 * Intérêt :
 * Tester le mécanisme de création et d'exécution des threads utilisateurs.
 * dans Nachos avec plusieurs threads en parallèle avec getchar
 */

#include "syscall.h"

void fc (void *arg) {
    char x;
    int i;
    (void)arg;
    x=GetChar();
    for (i = 0; i < 5; i++) {
        PutChar(x);          //affiche le caractère pointé par x
    }
    PutChar('\n');
    ThreadExit();         //termine l'exécution du thread courant 
}

int main () {

    ThreadCreate(fc, 0); //crée un Thread
    ThreadCreate(fc, 0); //crée un deuxième Thread
    while(1);             //afin que le thread s'exécute avant la fin du programme principal
    return 0;
}