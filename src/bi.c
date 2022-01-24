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
        for(u16 a = 1; a < argc; a++){
            // outfile
            if(!strcmp(args[a], "-o"))
                if(outf == nil)
                    if(a + 1 <= argc - 2) outf = args[++a];
                    else cmperr("illegal use of argument", nil, nil);
                else
                    cmperr("multiple out-file definitions", nil, nil);

            // compilation method
            else
            if(!strcmp(args[a], "-r") // release
            or !strcmp(args[a], "-d") // debug
            ){
                if(mthd == nil){
                    if(strlen(args[a]) == 3) nasm = T;
                    mthd = args[a];
                }
                else
                cmperr("compilation method defined multiple times", nil, nil);
            }
            else if(!strcmp(args[a], "-s"))
                if(!strp) strp = T;
                else cmperr("compilation flag defined multiple times", nil, nil);

            else if(!strcmp(args[a], "-S"))
                if(!nasm) nasm = T;
                else cmperr("compilation flag defined multiple times", nil, nil);

            // compilation flags
            else if(!strcmp(args[a], "--trackidx"))
                if(!tidx) tidx = T;
                else cmperr("compilation flag defined multiple times", nil, nil);
        }
        // check file
        if(access(args[argc - 1], F_OK))
            cmperr("the given file does not"
            " exist or cannot be read", nil, nil);
        else
        lddf = args[argc - 1];

        if(outf == nil) outf = "a.out";
        if(mthd == nil) mthd = "-d";
        fptr = fopen(lddf, "r");

        // null file
        if(fptr == nil)
            cmperr("something went wrong"
            " while reading the file", nil, nil);

        // prettify name
        char * temp = strrchr(lddf, '/');
        if(temp) lddf = temp + 1;
    }
    // compile
    comp(fptr, outf, lddf, mthd);

    return 0;
}