#ifndef STRLIB
    #include "strlib.h"
#endif

u64 strarrlen(char ** arr, u64 len){

    u64 out = 0;
    for(u64 i = 0; i < len; i++){out += sizeof(arr[i]);}
    return out;
}

i64 strfnd(char *str, const char *pat){
    // avoid a lot of calls
    u64 size = strlen(str);
    u64 ptsz = strlen(pat);

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offsetted string
    char sub[ptsz + 1];
    // loop until the end
    for(i64 o = 0; o + ptsz <= size; o++){
        // create substring
        for(i64 c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) return o;
    }
    return -1;
}

// find last occurrence
i64 strfndl(char *str, const char *pat){
    // avoid a lot of calls
    u64 size = strlen(str);
    u64 ptsz = strlen(pat);

    i64 pos = -1;

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offsetted string
    char sub[ptsz + 1];
    // loop until the end
    for(i64 o = 0; o + ptsz <= size; o++){
        // create substring
        for(i64 c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) pos = o;
    }
    return pos;
}

// offsetted search
i64 strafnd(char *str, const char *pat, i64 off){
    // avoid a lot of calls
    u64 size = strlen(str);
    u64 ptsz = strlen(pat);

    // check args
    if(off < 0) off = size + off;
    if(off > size){
        fprintf(stderr, "[%s, %s, %ld] index out of range"\
        " (%ld [offset] > %ld [strlen])\n",
        str, pat, off, off, size);
        exit(-1);
    }
    if(strlen(pat) > size) return -1;

    if(size == ptsz)
    if(!strcmp(str, pat)) return 0;

    // offsetted string
    char sub[ptsz + 1];
    // loop until the end
    for(i64 o = off; o + ptsz <= size; o++){
        // create substring
        for(i64 c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) return o;
    }
    return -1;
}

// count of matching cases
u64 strfndc(char *str, const char *pat){
    // avoid a lot of calls
    u64 size = strlen(str);
    u64 ptsz = strlen(pat);

    if(size == ptsz)
    if(!strcmp(str, pat)) return 1;
    else return 0;

    // offsetted string
    i64 cnnt = 0;
    char sub[ptsz + 1];
    // loop until the end
    for(i64 o = 0; o + ptsz <= size; o++){
        // create substring
        for(i64 c = o; c - o <= ptsz; c++){
            // move substring's char to offset
            sub[c - o] = str[c];
        }
        sub[ptsz] = '\0';
        if(!strcmp(sub, pat)) cnnt++;
    }
    return cnnt;
}

i64 str_arrfnd(char **arr, const char *val, u64 len){
    for(u16 i = 0; i < strarrlen(arr, len) / sizeof(arr); i++){
        // check every item in array
        if(!strcmp(arr[i], val)) return i;
    }
    return -1;
}

// stack-allocated arrlen find
i64 stk_strarrfnd(char **arr, char *val){
    for(u64 i = 0; i < sizeof(arr); i++){
        // check every item in array
        if(!strcmp(arr[i], val)) return i;
    }
    return -1;
}

// remember that str is freed after calling
char *str_sub(char *str, i64 i, i64 f){
    // sugar indexes
    u64 len = strlen(str);
    if(i < 0) i = len + i + 1;
    if(f < 0) f = len + f + 1;

    // invalid slices
    if(i < 0 or i > f){
        fprintf(stderr, "[args: %s, %ld, %ld] index out of range"\
        " (%ld [arg 0] > %ld [arg 1])\n", str,
        i, f, i > 0 ? i : 0, f > i ? i : f);

        exit(-1);
    }
    if(f > strlen(str) + 1){
        fprintf(stderr, "[args: %s, %ld, %ld] index out of range"\
        " (%ld [arg 1] > %ld [str len])\n",
        str, i, f, f, strlen(str));

        exit(-1);
    }
    // char slice
    if(i == f){
        char *out = alloc(2);
        sprintf(out, "%c", str[i]);
        return out;
    }

    char *out = alloc(f - i + 1);
    for(u64 c = i; c <= f; c++){out[c - i] = str[c];}

    out[f - i] = '\0';
    return out;
}

// remember that str is freed after calling
char *strgsub(char *str, const char *pat, char *sup){
    // safety check
    if(strfnd(str, pat) == -1) return str;
    if(!strcmp(str, pat)){
        free(str);
        char *out = alloc(2);
        strcpy(out, "");
        return out;
    }
    if(!strcmp(pat, sup)) return str;

    char *out =
    alloc(strlen(str) + max(strlen(sup), 1) * strfndc(str, pat) + 1);
    strcpy(out, str);

    u64 frst = strfnd(out, pat);
    while(frst != -1){
        u64 last;
        char *left, *rght;
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

char *strtrim(char *str){
    u64 len = strlen(str);

    bool started = F;
    u64 stt = 0, end = 0;
    for(u64 c = 0; c < len; c++){
        if(!started and str[c] == ' ') stt++;
        else
        if(!started) started = T;
        if(started and str[c] != ' ') end = c;
    }
    if(!(stt or end)) return str;

    char *out = str_sub(str, stt, end + 1);
    free(str);

    return out;
}

char *strpush(char *str, char *otr){
    char *out = alloc(strlen(str) + strlen(otr) + 1);
    strcpy(out, str);
    free(str);

    strcpy(out + strlen(out), otr);
    return out;
}