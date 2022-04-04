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

    // no arguments, so prompt help
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
        // what command is this?
        if( strcmp(args[1], "check")
        and strcmp(args[1], "debug")
        and strcmp(args[1], "build"))
            // invalid argument
            cmperr("invalid compiler argument.\n"\
            "please try running without args to get help.", nil, nil);

        // the minimum of arguments when compiling is 3
        if(argc < 3) cmperr("source file is missing", nil, nil);

        // check files
        for(u64 f = 2; f < argc; f++){
            if(access(args[f], F_OK))
                cmperr("the given file does not"
                " exist or cannot be read", nil, nil);

            // TODO: check other arguments and work properly with them
            // * expected files: .o, .s, .bi
        }

        // create a mutable and freeable copy of the file name
        char * lddf = malloc(strlen(args[2]) + 1);
        strcpy(lddf, args[2]);

        // the main module file is the first file
        fptr = fopen(lddf, "r");

        // no file found
        if(fptr == nil)
            cmperr("something went wrong"
            " while reading the file", nil, nil);

        // prettify name so then the compiler knows the module name
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