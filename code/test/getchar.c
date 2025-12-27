/*
 * Usage : ./nachos -x ../test/putchar
 * 
 * Intérêt :
 * Test de l'appel système GetChar
 * Chaque caractère lu au clavier est réaffiché, 
 * le programme s'arrête quand le code de retour est -1 (EOF)
*/

#include "syscall.h"

int main() {
    for (;;) {              //boucle infini
        char c;
        c = GetChar();      //lit un caractère
        if (c == -1) break; //si EOF sortir de la boucle
        PutChar(c);         //sinon on affiche le caractère
    }
    return 0;
}