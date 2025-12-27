/**
 * Usage : ./nachos -x ../test/getstring
 * 
 * Intérêt :
 * Test de différents cas d'utilisation de l'appel système GetString.
 * 1 - lecture avec un buffer normal
 * 2 - lecture avec un buffer petit(gestion de troncature)
 * 3 - lecture avec un buffer taille 1 on ne peut stocker aucun caractère
 */

 #include "syscall.h"

 int main() {
   char buf1[32];
   char buf2[8];
   char buf3[1];

    //1 - lecture avec un buffer normal
    
    PutString("Entrez une phrase (max 31 caractères) :\n");
    GetString(buf1, 32);
    PutString("Lu :");
    PutString(buf1);
    PutChar('\n');

    //2 - lecture avec un buffer petit(gestion de troncature)
    
    PutString("Entrez une phrase (max 7 caractères) :\n");
    GetString(buf2, 8);
    PutString("Lu :");
    PutString(buf2);
    PutChar('\n');

    //3 - lecture avec un buffer taille 1 on ne peut stocker aucun caractère
    

    PutString("\nTest avec max 0 caractère :\n");
    GetString(buf3, 0);
    PutString("Lu :");
    PutString(buf3);
    PutChar('\n');

    return 0;
 }