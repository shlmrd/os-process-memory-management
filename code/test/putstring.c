/*
 * Usage : ./nachos -x ../test/putstring
 *
 * Intérêt :
 * Ce test regroupe plusieurs cas de PutString :
 * 1 - Chaîne courte
 * 2 - Chaîne vide ("")
 * 3 - Chaîne pile MAX_STRING_SIZE-1 qui est ici 8-1
 * 4 - Chaîne plus longue
 * 5 - Chaîne avec juste "\n"
 *
 * La sortie doit correspondre exactement aux différentes chaînes sans crash
 * ni dépassement mémoire (vérifié avec Valgrind/ASan).
 */

 #include "syscall.h"

 int main() {
    //1 - Chaîne courte
    PutString("Test ");

    //2 - Chaîne vide ("")
    PutString("");

    //3 - Chaîne pile MAX_STRING_SIZE-1
    PutString("Bonjour");

    //4 - Chaîne plus longue
    PutString(" Voila des exemples de notre implémentation");

    //5 - Chaîne avec juste "\n"
    PutString("\n");
    
    //après l'implémentation de Exit on n'a plus besoin d'appeler Halt(), on peut utiliser return n
    return 0;
 }