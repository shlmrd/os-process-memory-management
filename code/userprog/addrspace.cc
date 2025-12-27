// addrspace.cc
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "syscall.h"
#include "new"
#ifdef CHANGED
#include "pageprovider.h"
#include "synch.h"
#endif//CHANGED
//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------
#ifdef CHANGED
#define Pile_size 256
#endif

#ifdef CHANGED
static void
ReadAtVirtual (OpenFile *executable, int virtualaddr,int numBytes, int position, TranslationEntry *pageTable, unsigned numPages)
{
    char buf[numBytes];
    executable->ReadAt(buf, numBytes, position);

    TranslationEntry *currentPageTable = machine->currentPageTable;
    unsigned int currentPageTableSize = machine->currentPageTableSize;

    machine->currentPageTable = pageTable;
    machine->currentPageTableSize = numPages;

    for (int i = 0; i < numBytes; i++){
        machine->WriteMem(virtualaddr+i, 1, buf[i]);
    }

    machine->currentPageTable = currentPageTable;    
    machine->currentPageTableSize = currentPageTableSize;
}
#endif //CHANGED

static void
SwapHeader (NoffHeader * noffH)
{
    noffH->noffMagic = WordToHost (noffH->noffMagic);
    noffH->code.size = WordToHost (noffH->code.size);
    noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
    noffH->initData.size = WordToHost (noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost (noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
        WordToHost (noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpaceList
//      List of all address spaces, for debugging
//----------------------------------------------------------------------
List AddrSpaceList;

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------


AddrSpace::AddrSpace (OpenFile * executable)
{
    unsigned int size;

    executable->ReadAt (&noffH, sizeof (noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost (noffH.noffMagic) == NOFFMAGIC))
        SwapHeader (&noffH);
    /* Check that this is really a MIPS program */
    ASSERT_MSG (noffH.noffMagic == NOFFMAGIC, "This is not a nachos binary!\n");

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStacksAreaSize;	// we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp (size, PageSize);
    size = numPages * PageSize;

    // check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory
    if (numPages > NumPhysPages)
            throw std::bad_alloc();

    DEBUG ('a', "Initializing address space, num pages %d, total size 0x%x\n",
           numPages, size);
    #ifdef CHANGED
//utilisation de pageprovider
	ASSERT((int) numPages <= pageProvider->NumAvailPage());

	pageTable = new TranslationEntry[numPages];

	for (unsigned int i = 0; i < numPages; i++) {
		int physPage = pageProvider->GetEmptyPage();
    		ASSERT(physPage != -1);

    		pageTable[i].virtualPage = i;
		pageTable[i].physicalPage = physPage;
    		pageTable[i].valid = TRUE;
    		pageTable[i].use = FALSE;
    		pageTable[i].dirty = FALSE;
    		pageTable[i].readOnly = FALSE;
}

    #endif //CHANGED

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0)
      {
        DEBUG ('a', "Initializing code segment, at 0x%x, size 0x%x\n",
               noffH.code.virtualAddr, noffH.code.size);
    #ifdef CHANGED
        ReadAtVirtual (executable, noffH.code.virtualAddr,
                      noffH.code.size, noffH.code.inFileAddr, pageTable, numPages);
    #else
         executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),
                            noffH.code.size, noffH.code.inFileAddr);
    #endif
      }
    if (noffH.initData.size > 0)
      {
        DEBUG ('a', "Initializing data segment, at 0x%x, size 0x%x\n",
               noffH.initData.virtualAddr, noffH.initData.size);
    #ifdef CHANGED
        ReadAtVirtual (executable, noffH.initData.virtualAddr,
                      noffH.initData.size, noffH.initData.inFileAddr, pageTable, numPages);
      }
    #else
        executable->ReadAt (&(machine->mainMemory[noffH.initData.virtualAddr]),
                            noffH.initData.size, noffH.initData.inFileAddr);
    #endif


    DEBUG ('a', "Area for stacks at 0x%x, size 0x%x\n",
           size - UserStacksAreaSize, UserStacksAreaSize);

    pageTable[0].valid = FALSE;			// Catch NULL dereference

    AddrSpaceList.Append(this);
#ifdef CHANGED
    threadCount = 1;
    threadCountSem = new Semaphore ("threadCountSem",1);
    
    int nbPiles = UserStacksAreaSize / Pile_size;
    stackBitmap = new BitMap(nbPiles);
    stackBitmap->Mark(0);  // le thread principal occupe le premier slot
    DEBUG('s', "Bitmap des piles initialisée (%d slots disponibles)\n", nbPiles);
    machine->DumpMem("addrspace.svg");
#endif //CHANGED
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
#ifdef CHANGED
  for (unsigned int i = 0; i < numPages; i++) {
     pageProvider->ReleasePage(pageTable[i].physicalPage);
  }
  delete threadCountSem;
  threadCountSem = NULL;
#endif //CHANGED
  delete [] pageTable;
  pageTable = NULL;

  AddrSpaceList.Remove(this);
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters ()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister (i, 0);

    // Initial program counter -- must be location of the __start function
    machine->WriteRegister (PCReg, USER_START_ADDRESS);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister (NextPCReg, machine->ReadRegister(PCReg) + 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister (StackReg, numPages * PageSize - 16);
    DEBUG ('a', "Initializing stack register to 0x%x\n",
           numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::Dump
//      Dump program layout as SVG
//----------------------------------------------------------------------

static void
DrawArea(FILE *output, unsigned sections_x, unsigned virtual_x,
         unsigned y, unsigned blocksize,
         struct segment *segment, const char *name)
{
    if (segment->size == 0)
        return;

    ASSERT((segment->virtualAddr % PageSize == 0));
    ASSERT((segment->size % PageSize == 0));
    unsigned page = segment->virtualAddr / PageSize;
    unsigned end = (segment->virtualAddr + segment->size) / PageSize;

    fprintf(output, "<rect x=\"%u\" y=\"%u\" width=\"%u\" height=\"%u\" "
                    "fill=\"#ffffff\" "
                    "stroke=\"#000000\" stroke-width=\"1\"/>\n",
                    sections_x, y - end * blocksize,
                    virtual_x - sections_x, (end - page) * blocksize);

    fprintf(output, "<text x=\"%u\" y=\"%u\" fill=\"#000000\" font-size=\"%u\">%s</text>\n",
            sections_x, y - page * blocksize, blocksize, name);
}

unsigned
AddrSpace::Dump(FILE *output, unsigned addr_x, unsigned sections_x, unsigned virtual_x, unsigned virtual_width,
                unsigned physical_x, unsigned virtual_y, unsigned y,
                unsigned blocksize)
{
    unsigned ret = machine->DumpPageTable(output, pageTable, numPages,
            addr_x, virtual_x, virtual_width, physical_x, virtual_y, y, blocksize);

    DrawArea(output, sections_x, virtual_x, virtual_y, blocksize, &noffH.code, "code");
    DrawArea(output, sections_x, virtual_x, virtual_y, blocksize, &noffH.initData, "data");
    DrawArea(output, sections_x, virtual_x, virtual_y, blocksize, &noffH.uninitData, "bss");

    DumpThreadsState(output, this, sections_x, virtual_x, virtual_y, blocksize);

    return ret;
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpacesRoom
//      Return how much room is needed for showing address spaces
//----------------------------------------------------------------------

unsigned
AddrSpacesRoom(unsigned blocksize)
{
    ListElement *element;
    unsigned room = 0;

    for (element = AddrSpaceList.FirstElement ();
         element;
         element = element->next) {
        AddrSpace *space = (AddrSpace*) element->item;
        room += machine->PageTableRoom(space->NumPages(), blocksize);
    }

    return room;
}

//----------------------------------------------------------------------
// AddrSpace::DumpAddrSpaces
//      Dump all address spaces
//----------------------------------------------------------------------

void
DumpAddrSpaces(FILE *output,
               unsigned addr_x, unsigned sections_x, unsigned virtual_x, unsigned virtual_width,
               unsigned physical_x, unsigned y, unsigned blocksize)
{
    ListElement *element;
    unsigned virtual_y = y;

    /* TODO: sort by physical page addresses to avoid too much mess */
    for (element = AddrSpaceList.FirstElement ();
         element;
         element = element->next) {
        AddrSpace *space = (AddrSpace*) element->item;
        virtual_y -= space->Dump(output, addr_x, sections_x, virtual_x, virtual_width, physical_x, virtual_y, y, blocksize);
    }
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

void
AddrSpace::SaveState ()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void
AddrSpace::RestoreState ()
{
    machine->currentPageTable = pageTable;
    machine->currentPageTableSize = numPages;
}

#ifdef CHANGED
int AddrSpace::AllocateUserStack(int slot) {
    int stackTop = numPages * PageSize;
    int stack = stackTop - (slot + 1) * Pile_size;//stackTop - (threadCount * Pile_size); ancienne version
    DEBUG('s', "nouvelle pile allouée slot %d, adresse = %d\n", slot, stack);
    return stack;
}

int AddrSpace::AddThread() {
    threadCountSem->P();

    // Trouver un slot libre dans la BitMap
    int slot = stackBitmap->Find();  // retourne un index libre (0, 1, 2, …)
    if (slot != -1) {
        threadCount++;
        stackBitmap->Mark(slot);     // marque le slot comme utilisé
        DEBUG('s', "Thread ajouté : slot %d utilisé (total = %d)\n", slot, threadCount);
    } else {
        DEBUG('s', "Aucune pile libre disponible pour un nouveau thread\n");
    }

    threadCountSem->V();
    return slot; // on renvoie le numéro de slot (ou -1 si plus de place)
}

int AddrSpace::RemoveThread(int slot) {
    threadCountSem->P();

    if (slot >= 0) {
        stackBitmap->Clear(slot);   
        threadCount--;
        DEBUG('s', "Thread supprimé : slot %d libéré (total = %d)\n", slot, threadCount);
    }
    threadCountSem->V();
    return threadCount;
}
#endif // CHANGED

