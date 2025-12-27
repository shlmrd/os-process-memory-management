/**
 * Usage : ./nachos -rs 1234 -x ../test/lockthreads_getstring
 * 
 * Intérêt :
 * Tester le mécanisme de création et d'exécution des threads utilisateurs.
 * dans Nachos avec plusieurs threads en parallèle avec getstring
 */

#include "syscall.h"

void fc (void *arg) {
    int i;
    char buf[64];
    (void)arg;
    GetString(buf,64);
    for (i = 0; i < 5; i++) {
        PutString(buf);   //affiche la chaine de caractère entrée
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