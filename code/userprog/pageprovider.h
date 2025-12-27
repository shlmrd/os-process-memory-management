#ifdef CHANGED

#ifndef PAGEPROVIDER_H
#define PAGEPROVIDER_H
#include "bitmap.h"
#include "copyright.h"
#include "utility.h"


class PageProvider:dontcopythis {
    public:
        PageProvider(int numPhysPage);
        ~PageProvider();
        int GetEmptyPage();
        void ReleasePage(int page);
        int NumAvailPage();
    private:
        BitMap *bitmap;
};

#endif // PAGEPROVIDER_H
#endif // CHANGED