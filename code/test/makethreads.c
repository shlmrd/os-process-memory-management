/**
 * Usage : ./nachos -rs 1234 -x ../test/makethreads
 * 
 * Intérêt :
 * Tester le mécanisme de création et d'exécution de threads utilisateurs
 * dans Nachos.
 * Ce test permet de vérifier le bon fonctionnement de :
 * - La création de threads via `ThreadCreate`
 * - L'exécution d'un thread utilisateur qui appelle `PutChar`
 */


#include "syscall.h"

void fc (void *nom) {
    char *x=(char *)nom;
    PutChar(*x);          //affiche le caractère pointé par x
    ThreadExit();         //quitte ce thread
}

int main () {
    char x='p';
    ThreadCreate(fc, &x); //crée un Thread
    while(1);             //afin que le thread s'exécute avant la fin du programme principal
    return 0;
}
