#include <string.h>

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef bcclib
    #include "bcclib.h"
#endif

size_t strarrlen(char ** arr, size_t len);

ssize_t  strfnd(char * str, const char * pat);
size_t  strfndc(char * str, const char * pat);              // count of finds
ssize_t strfndl(char * str, const char * pat);              // last found
ssize_t strafnd(char * str, const char * pat, ssize_t off); // offseted search

ssize_t str_arrfnd(char ** arr, const char * val, size_t len);
ssize_t stk_strarrfnd(char ** arr, char * val);

char * str_sub(char * str, ssize_t i , ssize_t f);
char * strgsub(char * str, const char * pat, char * sup);
char * strtrim(char * str);

#define startswith(str, pat) \
!strncmp(str, pat, strlen(pat))

#define endswith(str, pat) \
!strcmp(str + strlen(str) - min(strlen(pat), strlen(str)), pat)

#include "strlib.c"