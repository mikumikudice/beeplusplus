#include <string.h>

#ifndef bcclib
    #include "bcclib.h"
#endif

#ifndef rustt
    #include "rust.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _CTYPE_H
    #include <ctype.h>
#endif

u64 strarrlen(char ** arr, u64 len);

i64  strfnd(char * str, const char * pat);
u64  strfndc(char * str, const char * pat);              // count of finds
i64 strfndl(char * str, const char * pat);              // last found
i64 strafnd(char * str, const char * pat, i64 off); // offseted search

i64 str_arrfnd(char ** arr, const char * val, u64 len);
i64 stk_strarrfnd(char ** arr, char * val);

char * str_sub(char * str, i64 i , i64 f);
char * strgsub(char * str, const char * pat, char * sup);
char * strtrim(char * str);

char * strpush(char * str, char * otr);

#define startswith(str, pat) \
!strncmp(str, pat, strlen(pat))

#define endswith(str, pat) \
!strcmp(str + strlen(str) - min(strlen(pat), strlen(str)), pat)

#include "strlib.c"