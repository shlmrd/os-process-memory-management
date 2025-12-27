/**
* Usage : ./nachos -rs 123 -x ../test/forkthreads
*
* Vérifier la gestion simultanée d’un grand nombre de processus
* utilisateurs, chacun possédant ses propres threads. Ce programme
* crée plusieurs processus via ForkExec, chacun exécutant userpages1,
* qui lui-même lance plusieurs threads utilisateurs.
*/
#include "syscall.h"

int main ()
{
    int i;
    for(i=0; i<12; i++){
        ForkExec("../test/userpages1");
    }
    Exit(0);
}
