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

size_t strarrlen(char ** arr, size_t len){

    size_t out = 0;
    for(size_t i = 0; i < len; i++){out += sizeof(arr[i]);}
    return out;
}

ssize_t strfnd(char * str, const char * pat){
    // avoid a lot of calls
    size_t size = strlen(str);
    size_t ptsz = strlen(pat);

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offseted string
    char sub[ptsz + 1];
    // loop until the end
    for(ssize_t o = 0; o + ptsz <= size; o++){
        // create substring
        for(ssize_t c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) return o;
    }
    return -1;
}

// find last occurrence
ssize_t strfndl(char * str, const char * pat){
    // avoid a lot of calls
    size_t size = strlen(str);
    size_t ptsz = strlen(pat);

    ssize_t pos = -1;

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offseted string
    char sub[ptsz + 1];
    // loop until the end
    for(ssize_t o = 0; o + ptsz <= size; o++){
        // create substring
        for(ssize_t c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) pos = o;
    }
    return pos;
}

// offseted search
ssize_t strafnd(char * str, const char * pat, ssize_t off){
    // avoid a lot of calls
    size_t size = strlen(str);
    size_t ptsz = strlen(pat);

    // check args
    if(off < 0) off = size + off;
    if(off > size){
        fprintf(stderr, "[%s, %s, %d] index out of range"\
        " (%d [offset] > %d [strlen])\n",
        str, pat, off, off, size);
        exit(-1);
    }
    if(strlen(pat) > size) return -1;

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offseted string
    char sub[ptsz + 1];
    // loop until the end
    for(ssize_t o = off; o + ptsz <= size; o++){
        // create substring
        for(ssize_t c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) return o;
    }
    return -1;
}

// count of matching cases
size_t strfndc(char * str, const char * pat){
    // avoid a lot of calls
    size_t size = strlen(str);
    size_t ptsz = strlen(pat);

    if(size == ptsz)
    if(!strcmp(str, pat)) return 1;
    else return 0;

    // offseted string
    ssize_t cnnt = 0;
    char sub[ptsz + 1];
    // loop until the end
    for(ssize_t o = 0; o + ptsz <= size; o++){
        // create substring
        for(ssize_t c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) cnnt++;
    }
    return cnnt;
}

ssize_t str_arrfnd(char ** arr, const char * val, size_t len){
    for(u16 i = 0; i < strarrlen(arr, len) / sizeof(arr); i++){
        // check every item in array
        if(!strcmp(arr[i], val)) return i;
    }
    return -1;
}

// stack-allocated arrlen find
ssize_t stk_strarrfnd(char ** arr, char * val){

    for(size_t i = 0; i < sizeof(arr); i++){
        // check every item in array
        if(!strcmp(arr[i], val)) return i;
    }
    return -1;
}


// remember that str is freed after calling
char * str_sub(char * str, ssize_t i, ssize_t f){
    // sugar indexes
    if(i < 0) i = strlen(str) + i + 1;
    if(f < 0) f = strlen(str) + f + 1;

    // invalid slices
    if(i < 0 or i > f){
        fprintf(stderr, "[%s, %d, %d] index out of range"\
        " (%d [arg 0] > %d [arg 1])\n", str,
        i, f, i > 0 ? i : 0, f > i ? i : f);

        exit(-1);
    }
    if(f > strlen(str) + 1){
        fprintf(stderr, "[%s, %d, %d] index out of range"\
        " (%d [arg 1] > %d [str len])\n",
        str, i, f, f, strlen(str));

        exit(-1);
    }
    // char slice
    if(i == f){
        char * out = malloc(2);
        sprintf(out, "%c", str[i]);
        return out;
    }

    char * out = malloc(f - i + 1);
    for(size_t c = i; c <= f; c++){out[c - i] = str[c];}

    out[f - i] = '\0';
    return out;
}

// remember that str is freed after calling
char * strgsub(char * str, const char * pat, char * sup){
    // safety check
    if(strfnd(str, pat) == -1) return str;
    if(!strcmp(str, pat)){
        free(str);
        return "";
    }
    if(!strcmp(pat, sup)) return str;

    char * out =
    malloc(strlen(str) + max(strlen(sup), 1) * strfndc(str, pat) + 1);
    strcpy(out, str);

    size_t frst = strfnd(out, pat);
    while(frst != -1){
        size_t last;
        char * left, *rght;
        // split the string in what comes before
        // the pattern and what comes after it
        last = frst + strlen(pat);

        // check indexes before assigning
        if(frst != 0) left = str_sub(out, 0, frst);
        else left = "";

        if(last != strlen(out)) rght = str_sub(out, last, -1);
        else rght = "";

        sprintf(out, "%s%s%s", left, sup, rght);
        if(!isblnk(left)) free(left);
        if(!isblnk(rght)) free(rght);

        frst = strfnd(out, pat);
    }
    free(str);
    return out;
}

char * strtrim(char * str){
    size_t len = strlen(str);

    bool started = F;
    size_t stt = 0, end = 0;
    for(size_t c = 0; c < len; c++){
        if(!started and str[c] == ' ') stt++;
        else
        if(!started) started = T;
        if(started and str[c] != ' ') end = c;
    }
    if(!(stt or end)) return str;

    char * out = str_sub(str, stt, end + 1);
    free(str);

    return out;
}