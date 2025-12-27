#ifdef CHANGED

#include "copyright.h"
#include "pageprovider.h"
#include "system.h"

PageProvider::PageProvider(int numPhysPage)
{
    bitmap = new BitMap(numPhysPage);
}

PageProvider::~PageProvider()
{
    delete bitmap;
}

int
PageProvider::GetEmptyPage()
{
    int init = bitmap->Find();
    if(init == -1) return -1;
    memset(&(machine->mainMemory[init * PageSize]),0,PageSize);
    return init;
}

void
PageProvider::ReleasePage(int page)
{
    bitmap->Clear(page);
}

int
PageProvider::NumAvailPage()
{
    return bitmap->NumClear();
}

#endif //CHANGED