// exception.cc
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#ifdef CHANGED
#include "userthread.h"
#endif //CHANGED
//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}

#ifdef CHANGED
//----------------------------------------------------------------------
// copyStringFromMachine : Copy a string from the user to a kernel buffer
//----------------------------------------------------------------------
unsigned 
copyStringFromMachine(int from, char *to, unsigned size){
    if(size == 0) return 0;
    unsigned i;
    for(i=0 ; i<size-1 ; i++)
    {
      int val;
      bool read = machine->ReadMem(from+i,1,&val);
      if (!read){
        break;
      }
      to[i] = (unsigned char)val;
      if (to[i] == '\0') return i; 
    }
    to[i] = '\0';
    return i;
}

//----------------------------------------------------------------------
// copyStringToMachine : Copy a string from the kernel to a user
//----------------------------------------------------------------------
unsigned 
copyStringToMachine(char *from, int to, unsigned size){
  if(!from || size == 0) return 0;

  unsigned i;
  for(i = 0; i < size-1; i++){
    char val = from[i];
    if(val == '\0') break;
    machine->WriteMem(to+i, 1, (int)(unsigned)val);
  }
  machine->WriteMem(to+i, 1, 0);
  return i;
}

#endif //CHANGED

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);
    int address = machine->ReadRegister (BadVAddrReg);

    switch (which)
      {
        case SyscallException:
          {
            switch (type)
              {
                case SC_Halt:
                  {
                    DEBUG ('s', "Shutdown, initiated by user program.\n");
                    interrupt->Powerdown ();
                    break;
                  }
                default:
                  {
                    ASSERT_MSG(FALSE, "Unimplemented system call %d\n", type);
                  }
              #ifdef CHANGED
                case SC_Exit: 
                {
                  int code = machine->ReadRegister(4);
                  DEBUG ('s', "Exit(%d).\n", code);
                  processCount--;
                  if (processCount == 0) {
                        interrupt->Powerdown();
                  } else {
                        currentThread->Finish();
                  }
                  break;
                }
                case SC_PutChar:
                 {
                  DEBUG ('s', "PutChar\n" );
                  char c = (char)machine->ReadRegister(4);
                  consoledriver->PutChar(c);
                  break;
                 }
                case SC_PutString:
                 {
                  DEBUG ('s', "PutString\n");
                  int from = machine->ReadRegister(4);
                  char *to = new char[MAX_STRING_SIZE];
                  while (true){
                    unsigned n = copyStringFromMachine(from, to, MAX_STRING_SIZE);
                    consoledriver->PutString(to);
                    from += n;
                    if (n < MAX_STRING_SIZE - 1){
                      break;
                    }
                  }
                  delete [] to;
                  break;
                 }
                case SC_GetChar:
                 {
                  DEBUG ('s', "GetChar\n");
                  int c = consoledriver->GetChar();
                  machine->WriteRegister(2, c);
                  break;
                 }
                case SC_GetString:
                 {
                  DEBUG ('s', "GetString\n");
                  int toUser = machine->ReadRegister(4);
                  int size = machine->ReadRegister(5);

                  if(size <= 0){
                    break;
                  }
                  char buffer[MAX_STRING_SIZE + 1];
                  int totalRead = 0;

                  while (size > 0) {
                      int taillebloc = (size > MAX_STRING_SIZE) ? MAX_STRING_SIZE : size;
                      consoledriver->GetString(buffer, taillebloc);
                      copyStringToMachine(buffer, toUser + totalRead, taillebloc);

                      int readLen = strlen(buffer);
                      totalRead += readLen;
                      // Fin de lecture si on a lu moins que taillebloc
                      if (readLen < taillebloc)
                          break;

                      size -= taillebloc;
                  }
                  break;
                }
                 case SC_PutInt:
                {
                      DEBUG('s', "PutInt\n");
                      int n = machine->ReadRegister(4); 
                      char buf[12];                           
                      snprintf(buf, sizeof(buf), "%d", n);
                      consoledriver->PutString(buf);
                      break;
                  }

                  case SC_GetInt:
                  {
                      DEBUG('s', "GetInt\n");
                      int userAddr = machine->ReadRegister(4); 
                      char buf[32];
                      consoledriver->GetString(buf, sizeof(buf));

                      int value = 0;
                      if (sscanf(buf, "%d", &value) != 1) {   
                          value = 0;
                      }
                      machine->WriteMem(userAddr, sizeof(int), value);
                      break;
                  }

                  case SC_ThreadCreate:
                  {
                    int f = machine->ReadRegister(4);
                    int arg = machine->ReadRegister(5);
                    int val = do_ThreadCreate(f, arg);
                    machine -> WriteRegister(2, val);
                    break;
                  }

                  case SC_ThreadExit:
                  {
                    do_ThreadExit();
                    break;
                  }

                  case SC_ForkExec:
                  {
                    int from = machine->ReadRegister(4);
                    char file[1000];
                    copyStringFromMachine(from, file, 1000);
                    int val = do_ForkExec(file);
                    machine -> WriteRegister(2, val);
                    break;
                  }

              #endif //CHANGED
              }

            // Do not forget to increment the pc before returning!
            // This skips over the syscall instruction, to continue execution
            // with the rest of the program
            UpdatePC ();
            break;
          }

        case PageFaultException:
          if (!address) {
            ASSERT_MSG (FALSE, "NULL dereference at PC %x!\n", machine->registers[PCReg]);
          } else {
            // For now
            ASSERT_MSG (FALSE, "Page Fault at address %x at PC %x\n", address, machine->registers[PCReg]);
          }
          break;

        case ReadOnlyException:
          // For now
          ASSERT_MSG (FALSE, "Read-Only at address %x at PC %x\n", address, machine->registers[PCReg]);
          break;

        case BusErrorException:
          // For now
          ASSERT_MSG (FALSE, "Invalid physical address at address %x at PC %x\n", address, machine->registers[PCReg]);
          break;

        case AddressErrorException:
          // For now
          ASSERT_MSG (FALSE, "Invalid address %x at PC %x\n", address, machine->registers[PCReg]);
          break;

        case OverflowException:
          // For now
          ASSERT_MSG (FALSE, "Overflow at PC %x\n", machine->registers[PCReg]);
          break;

        case IllegalInstrException:
          // For now
          ASSERT_MSG (FALSE, "Illegal instruction at PC %x\n", machine->registers[PCReg]);
          break;

        default:
          ASSERT_MSG (FALSE, "Unexpected user mode exception %d %d %x at PC %x\n", which, type, address, machine->registers[PCReg]);
          break;
      }
}

