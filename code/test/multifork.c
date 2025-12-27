/**
* Usage: ./nachos -rs 123 -x ../test/multifork
*
* Ce test permet de vérifier la capacité du système à créer plusieurs processus
* utilisateurs successivement à l’aide de ForkExec. Ce test permet
* de confirmer que le noyau :
*     - instancie correctement un nouvel espace d’adressage (AddrSpace)
*       pour chaque processus lancé
*     - gère convenablement l’allocation mémoire (pages physiques)
*     - maintient correctement processCount
*     - termine proprement lorsque tous les processus ont fini.
*/

#include "syscall.h"

int main()
{
    int i;
    for(i=0; i<5; i++){
        ForkExec("../test/userpages0");
    }
    Exit(0);
}
