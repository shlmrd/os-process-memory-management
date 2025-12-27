 /**
  * Usage : ./nachos -x ../test/userpages0
  *
  * Ce test permet de vérifier la création et l’exécution d’un processus utilisateur minimal
  * dans Nachos.
  */

#include "syscall.h"

int main() {
    PutString("Test\n");
    Exit(0);
}
