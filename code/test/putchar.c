#include "syscall.h"
void print(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) {
    PutChar(c + i);
    }
    PutChar('\n');
}

int main()
{
    print('a',4);
    //après l'implémentation de Exit on n'a plus besoin d'appeler Halt(), on peut utiliser return n
    //Halt();
    return 0;
}