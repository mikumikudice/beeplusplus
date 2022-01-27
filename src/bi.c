#include "cmmn.h"
#include  "bic.h"

#define CVER "0.0.1"
#define DATE "2021-2022"

#define TUTL "https://github.com/mateus-md/beeplusplus/blob/master/doc/Tutorial/"

#include <unistd.h>

int main(i32 argc, char ** args){
    FILE * fptr;
    bool   nasm = F, tidx = F, opts = T, strp = F;
    char * lddf, * outf = nil, * mthd = nil;

    if(argc == 1){
        printf("B++ (incremented B) Compiler v%s - %s "\
        "copyright(c) Mateus M. D. Souza\n", CVER, DATE);

        puts("USAGE:\n\n bi [action] [main file] [other files]\n");
        puts("[action]'s:\n\n check: only checks the syntax and highlights errors"\
            "\n debug: compiles the source in debug mode"\
            "\n build: compiles the source in release mode\n"
        );
        printf(
            "NOTE:\n to change output file name or give"\
            " compiler args, put in the source file.\nsee"\
            " the docs (%s)\nto know how to do so.\n", TUTL
        );
        return 0;
    } else {
        if( strcmp(args[1], "check")
        and strcmp(args[1], "debug")
        and strcmp(args[1], "build"))
            // invalid argument
            cmperr("invalid compiler argument.\n"\
            "please try running without args to get help.", nil, nil);

        if(argc < 3) cmperr("source file is missing", nil, nil);

        // check files
        for(u64 f = 2; f < argc; f++){
            if(access(args[f], F_OK))
                cmperr("the given file does not"
                " exist or cannot be read", nil, nil);
            // TODO: compile all files and link them
        }

        u64 len = strlen(args[2]);
        char * lddf = malloc(len + 1);
        memcpy(lddf, args[2], len);
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
        cout * nasm = comp(fptr, temp, args[1]);
        free(lddf);

        free(nasm->outn);
        free(nasm);
    }
    return 0;
}