#include "cmmn.h"
#include  "bic.h"

#define CVER "0.0.1"
#define DATE "2021-2022"

#include <unistd.h>

int main(i32 argc, char ** args){
    FILE * fptr;
    bool   nasm = F, tidx = F, opts = T, strp = F;
    char * lddf, * outf = nil, * mthd = nil;

    if(argc == 1){
        printf("B++ (incremented B) Compiler v%s - %s "\
        "copyright(c) Mateus M. D. Souza\n", CVER, DATE);
        puts("USAGE:\n\n bi <comp args> [filename]");

        puts("\ncomp args:\n");
        puts("-o : build name; sets the output file name");
        puts("-r : release build; generates optimized artifacts");
        puts("-d : debug build (default); generates unoptimized artifacts");
        puts("-S : generates an asm file not assembled");

        return 0;
    } else {
        // check files
        for(u64 f = 1; f < argc; f++){
            if(access(args[f], F_OK))
                cmperr("the given file does not"
                " exist or cannot be read", nil, nil);
            // TODO: compile all files and link them
        }

        u64 len = strlen(args[1]);
        char * lddf = malloc(len + 1);
        memcpy(lddf, args[1], len);
        lddf[len] = '\0';

        fptr = fopen(lddf, "r");
        // null file
        if(fptr == nil)
            cmperr("something went wrong"
            " while reading the file", nil, nil);
        // prettify name
        char * temp = strrchr(lddf, '/');
        if(temp) temp = temp + 1;

        // compile
        cout * nasm = comp(fptr, temp);
        free(lddf);

        free(nasm->outn);
        free(nasm);
    }
    return 0;
}