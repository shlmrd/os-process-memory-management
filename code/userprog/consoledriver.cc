#ifdef CHANGED

#include "copyright.h"
#include "system.h"
#include "consoledriver.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Lock *inLock;
static Lock *outLock;
static Lock *string_inLock;
static Lock *string_outLock;

static void ReadAvailHandler(void *arg) { (void) arg; readAvail->V(); }
static void WriteDoneHandler(void *arg) { (void) arg; writeDone->V(); }

ConsoleDriver::ConsoleDriver(const char *in, const char *out)
{
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    inLock = new Lock("In Lock");
    outLock = new Lock("Out Lock");
    string_inLock = new Lock("String In Lock");
    string_outLock = new Lock("String Out Lock");
    console = new Console (in, out, ReadAvailHandler, WriteDoneHandler, NULL);
}

ConsoleDriver::~ConsoleDriver()
{
    delete console;
    delete writeDone;
    delete readAvail;
    delete inLock;
    delete outLock;
}

void ConsoleDriver::PutChar(int ch)
{
    outLock->Acquire();
    console->TX(char(ch));
    writeDone->P ();
    outLock->Release();
}

int ConsoleDriver::GetChar()
{   
    inLock->Acquire();
    readAvail->P ();
    inLock->Release();
    return console->RX ();
}

void ConsoleDriver::PutString(const char *s)
{
    if (s == nullptr){
        return;
    }
    string_outLock->Acquire();
    for(size_t i=0; i<strlen(s) ; i++){
        PutChar(s[i]);
    }
    string_outLock->Release();
}

void ConsoleDriver::GetString(char *s, int n)
{
    if (!s || n <= 0) return;
    if (n == 1) {
        s[0] = '\0';
        return;
    }
    string_inLock->Acquire();
    int i = 0;
    for (; i < n - 1; i++) {
        int c = GetChar();
        if (c == EOF) {
            if (i == 0) s[0] = '\0';
            break;
        }
        
        s[i] = (char)c;
        if (c == '\n') {
            i++;
            break;
        }
    }
    s[i] = '\0';
    string_inLock->Release();
}
#endif // CHANGED