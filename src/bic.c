#define LSIZE (64 << 1) // max of bytes read per line

// error messages
imut char * REALERR = "an unexpected error occurred";
imut char * UNEXPCT = "unexpected symbol found here";
imut char * UNCLSTR = "unclosed string found here";
imut char * UNCCMMT = "unclosed multiline comment found here";
imut char * UNCBRCK = "unclosed bracket found here";
imut char * UNCPARN = "unclosed parentheses found here";
imut char * CALLERR = "attempt to call an undefined function";
imut char * TOOFEWC = "too few arguments in the statement";
imut char * TOOMUCH = "too much arguments in the statement";
imut char * MULTIDF = "attempt to define multiple times the same namespace";
imut char * LITRIDX = "attempt to use a literal as namespace";
imut char * KWRDIDX = "attempt to use a keyword as namespace";

// array of source code lines
charr code;
u64 csz = LSIZE;

void comp(FILE * fptr, char * outf, char * lddf, char * mthd){
    // compiling timer
    clock_t crnt, oldt;
    oldt = clock();

    code.arr = malloc(sizeof(char *));
    COL(GRN);
    puts("+loading code  ...");
    COL(DEF);

    char l = 0, o = 0;
    i64 cmtl = 0;
    u64 i = 0;

    // get source
    bool isinc = F, isins = F, wait_err = F;
    for(char c; (c = fgetc(fptr));){
        // skip non textual chars
        if(c < 9 and c >= 0) continue;

        // enter multiline comment
        if(c == '*' and l == '/' and !isins){
            cmtl++;
            isinc = T;
        }
        // exit multiline comment
        if(c == '/' and l == '*' and !isins){
            cmtl--;
            if(cmtl == 0) isinc = F;
            else if(cmtl < 0 and !wait_err) wait_err = i;
        }
        // ignore symbols inside of strings
        if(isins){
            // it's a quote
            if(c == metachar[0].val[0] or c == metachar[1].val[0])
                // it's not scaped
                if(!isscpd(code.arr[code.len], i)){
                    // is inside or at the end of a string
                    // and matches with the openning char
                    if(isins and c == o) isins = F;
                    // it's the oppening char of an string/char literal
                    else if(!isins){
                        isins = T;
                        // remember the type of the quote
                        o = c;
                    }
                }
        }
        // last char is always updated
        l = c;

        // first char of line
        if(!i){
            // alloc line
            code.arr[code.len] = malloc(LSIZE);
            // resset length
            csz = LSIZE;
        }
        // grow line
        if(i >= csz) {
            csz += LSIZE / 2;
            code.arr[code.len] =
            realloc(code.arr[code.len], (csz) * sizeof(char *));

            if(!code.arr[code.len]) cmperr(REALERR, nil, nil);

            // first realloc
            if(csz == LSIZE * 1.5){
                assert(code.arr[code.len] == nil, UNEXPCT);
                tkn arrw = {
                    .line  = code.len,
                    .coll  = i,
                };
                char * msg = malloc(80);
                sprintf(msg, "line too long! greater than %s%d bytes%s", YEL, LSIZE, DEF);
                wrning(msg, &arrw, nil);
                free(msg);
            }
        }

        if(c != '\n' and c != EOF)
            code.arr[code.len][i++] = c;

        else {
            code.arr[code.len][i] = '\0';

            if(wait_err){
                tkn arrw = {
                    .line  = code.len,
                    .coll  = wait_err - 1,
                };
                cmperr(UNEXPCT, &arrw, nil);
            }
            i = 0;

            if(!isinc){
                if(code.arr[code.len][0] != '/'
                and code.arr[code.len][1] != '/')
                if(!hassym(code.arr[code.len])){
                    // default error arrow
                    tkn arrw = {
                        .line  = code.len,
                        .coll  = strlen(code.arr[code.len]) - 1
                    };
                    cmperr("expected expression or terminator", &arrw, nil);
                }

                // check for both quote types
                for(u16 s = 0; s < 2; s++){
                    u64 fidx = strfnd(code.arr[code.len], metachar[s].val);
                    if(fidx != -1){
                        // the first up to the last quote
                        u64 lidx = strfndl(code.arr[code.len], metachar[s].val);

                        tkn arrow = {
                            .line = code.len,
                            .coll = strlen(code.arr[code.len]) - 1
                        };
                        if(lidx == -1) cmperr(UNCLSTR, &arrow, nil);

                        // check count of quotes
                        u64 cnnt = 0;

                        // check if the quote count matches
                        for(u64 idx = fidx; idx <= lidx; idx++){
                            if(code.arr[code.len][idx] == metachar[s].val[0]
                            and code.arr[code.len][idx - 1] != '\\') cnnt++;
                        }

                        // missing quotes
                        if(cnnt % 2 != 0)
                        cmperr(UNCLSTR, &arrow, nil);
                    }
                }
            }
            // next line
            code.arr = realloc(code.arr, (code.len + 2) * sizeof(char *));
            if(!code.arr) cmperr(REALERR, nil, nil);

            code.len++;

            // check it here to avoid
            // skipping the last char
            if(c == EOF) break;
        }
    }
    fclose(fptr);

    COL(GRN);
    puts("+lexing code   ...");
    COL(DEF);

    tkn * tkns = lexit();

    COL(GRN);
    puts("+parsing code  ...");
    COL(DEF);

    // basic ast
    astt bast = parse(tkns);

    // TODO: final ast gen

    tkn * tok, * old = nil;
    u64   cnt = tkns->apdx;
    for(tok = tkns; cnt > 0; tok = tok->next, cnt--){
        // only free string values
        if(tok->type == INDEXER)
            free(tok->vall.str);

        // only free char literals
        else if(tok->type == LITERAL and tok->apdx == FREEABLE)
            free(tok->vall.str);

        if(old) free(old);
        old = tok;
    }
    free(old);

    // TODO: optimizations
    // TODO: nasm gen

    free(bast.ctxt);
    free_str();

    // compilation time
    crnt = clock();
    f32 dt = ((double)(crnt - oldt)) / CLOCKS_PER_SEC; 

    printf("-compiled %s into %s in ", lddf, outf);
    COL(GRN);
    printf("%.5fs\n", dt);
    COL(DEF);
}

void assert(bool check, imut char * msg){
    if(check){
        fprintf(stderr, RED);
        fprintf(stderr, "[ERROR]");
        fprintf(stderr, DEF);

        fprintf(stderr, ": %s\n", msg);
        exit(-1);
    }
}

void cmperr(imut char * err, tkn * arw, tkn * cmpl){
    // common prefix
    fprintf(stderr, RED);
    fprintf(stderr, "[ERROR]");
    fprintf(stderr, DEF);

    fprintf(stderr, " %s", err);
    if(arw)
    fprintf(stderr, " at %ld:%ld:\n", arw->line + 1, arw->coll + 1);
    else fprintf(stderr, "\n");

    if(arw){
        fprintf(stderr, BLU);
        fprintf(stderr, "\n\t%ld | %s\n\t", arw->line + 1, code.arr[arw->line]);
        fprintf(stderr, DEF);
        fflush(stderr);

        fprintf(stderr, RED);
        for(u16 i = 0; i < arw->coll + 4; i++){
            if(i > 3) fprintf(stderr, "~");
            else fprintf(stderr, " ");
        }
        fflush(stderr);

        fprintf(stderr, "^\n");
        fprintf(stderr, DEF);
    }
    if(cmpl != nil){
        fprintf(stderr, cmpl->vall.str);
        free(cmpl->vall.str);

        fprintf(stderr, " at %ld:%ld:\n",
        cmpl->line + 1, cmpl->coll + 1);

        fprintf(stderr, BLU);
        fprintf(stderr, "\n\t%ld | %s\n\t",
        cmpl->line + 1, code.arr[cmpl->line]);
        fprintf(stderr, DEF);
        fflush(stderr);

        fprintf(stderr, RED);
        for(u16 i = 0; i < cmpl->coll + 4; i++){
            if(i > 3) fprintf(stderr, "~");
            else fprintf(stderr, " ");
        }
        fflush(stderr);

        fprintf(stderr, "^\n");
        fprintf(stderr, DEF);
    }
    fprintf(stderr, "press any key to exit...");
    scanf("nothing");
    exit(-1);
}

void wrning(imut char * wrn, tkn * arw, tkn * cmpl){
    COL(YEL);
    printf("[WARNING]");
    COL(DEF);

    printf(" at %ld:%ld\n\t%s\n",
    arw->line, arw->coll, wrn);
}