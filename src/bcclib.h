#define bcclib

// Check if string is empty
#define isblnk(val) (!strcmp(val, ""))

#define arrlen(arr)  sizeof(arr) / sizeof(arr[0])

#define max(n, o) (n > o ? n : o)
#define min(n, o) (n > o ? o : n)

#ifndef __cplusplus

    #define and && // Better operators
    #define or  || // Better operators

    // Fortran-like boolean
    typedef enum boolean {F, T} bool;

    // nil for C
    #define nil ((void *) 0)
#else
    // nil for C++
    #define nil __null
#endif