/**
 * Usage: ./nachos -rs 123 -x ../test/forkexec
 *
 * Ce test permet de vérifier que l’appel système ForkExec fonctionne correctement
 */
#include "syscall.h"

int main()
{
    ForkExec("../test/putchar");
    ForkExec("../test/putchar");
    Exit(0);
}
