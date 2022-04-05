#ifndef bcclib
#include "bcclib.h"
#endif

#ifndef cmmn
#include "cmmn.h"
#endif

// same of malloc, but returns object zeroed
void *alloc(size_t size){
    void * pntr = malloc(size);
    memset(pntr, 0, size);

    return pntr;
}