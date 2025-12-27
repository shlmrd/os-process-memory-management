
/**
 * Usage : ./nachos -rs 1234 -x ../test/lockthreads_putstring
 * 
 * Intérêt :
 * Montrer que sans verrou dans PutString, plusieurs threads peuvent entrelacer leurs
 * affichages, même si PutChar est protégé.
 * Donc il faudrait protéger PutString par un verrou.
 */

#include "syscall.h"

void fc(void *arg) {
    const char *x = (const char*)arg;
    int i;
    for (i = 0; i < 10; i++){
        PutString(x);                 //affiche le caractère pointé par x
    }
    ThreadExit();                     //quitte ce thread
}
int main() {
    ThreadCreate(fc, "AAA\n");    //Thread secondaire
    fc("BBB\n");                  //Thread principal
    ThreadExit();                 //termine le thread principal, Nachos continue
    return 0;                     //jamais atteint
}