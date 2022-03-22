#define LSIZE (64 << 1) // max of bytes read per line

// error messages
imut char * REALERR = "an unexpected error occurred";
imut char * UNEXPCT = "unexpected symbol found here";
imut char * EXPCTDS = "expected expression or terminator";
imut char * UNCLSTR = "unclosed string found here";
imut char * UNCCMMT = "unclosed multiline comment found here";
imut char * UNCBRCK = "unclosed bracket found here";
imut char * UNCPARN = "unclosed parentheses found here";
imut char * EXPCTDP = "expected a parentheses at the end of this expression";
imut char * PRVONHR = "previously opened here";
imut char * ALONEXP = "expression out of context found here";
imut char * CALLERR = "attempt to call an undefined function";
imut char * TOOFEWC = "too few arguments in the statement";
imut char * TOOMUCH = "too much arguments in the statement";
imut char * MULTIDF = "attempt to define multiple times the same namespace";
imut char * LITRIDX = "attempt to use a literal as namespace";
imut char * KWRDIDX = "attempt to use a keyword as namespace";
imut char * KWRDVAL = "attempt to use a keyword as value";
imut char * EXPCTEX = "expected expression at this point";
imut char * DEFWOEQ = "you cannot assign a variable with shorthand operators while defining it";
imut char * NOTASGN = "this is not a valid assignment operator";
imut char * MSMATCH = "mismatch of number of assignators and assignateds in expression";
imut char * NOTERMN = "no terminator at the end of the line";

// array of source code lines
stra code;
u64 csz = LSIZE;

cout * comp(FILE * fptr, char * lddf, char * mode){
    // output
    cout * out = malloc(sizeof(out));

    char * dummy = malloc(strlen(lddf) + 1);
    strcpy(dummy, lddf);
    out->outn  = strgsub(dummy, ".bi", "");

    // compiling timer
    clock_t crnt, oldt = clock();
    f32 dt;

    code.arr = malloc(sizeof(char *));
    COL(GRN);
    printf("+loading code  ...");
    COL(DEF);

    char l = 0, o = 0;
    i64 cmtl = 0;
    u64 i = 0;

    // get source
    bool isstr = F;   // is the current sentence within a string?
    bool iscmt = F;   // is the current sentence a comment?
    bool isslc = F;   // the comment in question is single-lined?
    bool isscp = F;   // is the current content within a scope?
    bool ispar = F;   // is the current content within parentheses?

    i64  clvl  = 0;   // current comment nesting level
    i64  slvl  = 0;   // current scope level
    i64  plvl  = 0;   // current parentheses level

    u64 lcmt[csz << 2][2]; // keep track of where the last comment were oppened
    u64 lscp[csz << 2][2]; // keep track of where the last scope were oppened
    u64 lpar[csz << 2][2]; // keep track of where the last parentheses were oppened

    char last = 0;

    // init first line
    code.arr[code.len] = malloc(LSIZE);

    tkn arw;
    bool err = F;

    for(char c; (c = fgetc(fptr));){
        // skip non textual chars
        if(c < 9 and c >= 0) continue;

        // single-lined comment
        if(c == '/' and last == '/'){
            iscmt = T;
            isslc = T;
        // multiple-lined
        } else if(c == '*' and last == '/'){
            iscmt = T;
            lcmt[clvl][0] = code.len;
            lcmt[clvl][1] = i - 1;
            clvl++;
        // closing comment symbol
        } else if(c == '/' and last == '*'){
            clvl--;
            if(clvl == 0) iscmt = F;
            // number of closing symbols is greater than opening ones
            else if(clvl < 0){
                tkn arrow = {
                    .line = code.len,
                    .coll = i
                };
                code.arr[code.len][i++] = c;
                code.arr[code.len][i] = '\0';
                cmperr(UNEXPCT, &arrow, nil);
            }
            continue;
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

        // append code
        if(c != '\n' and c != EOF){
            if(!iscmt){
                code.arr[code.len][i++] = c;
                if(err) goto skip;

                // when coming back to check symbol, skip it
                if(last != c){
                    // take care of parenthesis and scopes
                    if(c == '{' and !isstr){
                        isscp = T;
                        lscp[plvl][0] = code.len;
                        lscp[plvl][1] = i - 1;
                        slvl++;
                    }
                    else if(c == '}' and !isstr){
                        slvl--;
                        if(!slvl) isscp = F;
                        else if(slvl < 0){
                            arw = (tkn){.line = code.len, .coll = i - 1};
                            err = T;
                        }
                    }

                    else if(c == '(' and !isstr){
                        ispar = T;
                        lpar[plvl][0] = code.len;
                        lpar[plvl][1] = i - 1;
                        plvl++;
                    }
                    else if(c == ')' and !isstr){
                        plvl--;
                        if(!plvl) ispar = F;
                        else if(plvl < 0){
                            arw = (tkn){.line = code.len, .coll = i - 1};
                            err = T;
                        }
                    }
                }
            } else if(i > 0){
                strcpy(code.arr[code.len], "");
                i = 0;
            }
            skip:
            last = c;
        // reached end of line
        } else {
            code.arr[code.len][i] = '\0';
            // catch the error at the end of the line
            if(err) cmperr(UNEXPCT, &arw, nil);

            if(!iscmt){
                // check it here to avoid
                // skipping the last char
                if(c == EOF){
                    code.len++;
                    break;
                }
            // end of line, end of comment
            } else if(isslc) iscmt = F;

            i = 0;
            // next line
            code.arr = realloc(code.arr, (code.len + 2) * sizeof(char *));
            if(!code.arr) cmperr(REALERR, nil, nil);

            // alloc next line
            code.arr[++code.len] = malloc(LSIZE);

            if(iscmt and !isslc) strcpy(code.arr[code.len], "");
            // resset length
            csz = LSIZE;
        }
    }
    fclose(fptr);

    // check for errors
    if(iscmt){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lcmt[clvl - 1][0],
            .coll = lcmt[clvl - 1][1],
            .vall.str = "previously started here"
        };
        cmperr(UNCCMMT, &tmp, &cmp);
    }
    else if(isscp){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lscp[slvl - 1][0],
            .coll = lscp[slvl - 1][1],
            .vall.str = "previously started here"
        };
        cmperr(UNCBRCK, &tmp, &cmp);
    } else if(ispar){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lpar[plvl - 1][0],
            .coll = lpar[plvl - 1][1],
            .vall.str = "previously started here"
        };
        printf("%d %d %d\n", plvl, plvl - 1, lpar[plvl - 1][1]);
        cmperr(UNCPARN, &tmp, &cmp);
    }

    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);
    printf(" done in %.5fs\n", dt);

    COL(GRN);
    printf("+lexing code   ...");
    COL(DEF);

    tkn * tkns = lexit();

    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);
    printf(" done in %.5fs\n", dt);

    COL(GRN);
    printf("+parsing code  ...");
    COL(DEF);

    cmod cmd;
    if(!strcmp(mode, "check")) cmd = CHECK;
    else if (!strcmp(mode, "debug")) cmd = DEBUG;
    else cmd = BUILD;

    // basic ast
    astt * bast = parse(tkns, cmd);
    if(cmd == CHECK) goto skip_cmp;

    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);
    printf(" done in %.5fs\n", dt);

    // TODO: final ast gen
    if(cmd == DEBUG) goto skip_opt;
    // TODO: optimizations
    skip_opt:
    // TODO: nasm gen

    skip_cmp:
    
    char * prnt;
    node * this = bast->pstt;
    while (T){
        prnt = nodet_to_str(this);
        printf("%s\n", prnt);
        free(prnt);
        if(this == bast->pend) break;
        this = this->next;
    }

    // free basic ast
    node * pntr = bast->pstt,
         * temp;
    u16 idx = 0;
    while(pntr != bast->pend){
       temp = pntr;
       pntr = temp->next;
       free(temp);
    }
    free(bast->pend);
    free(bast);
    free_str();

    tkn * tok, * old = nil;
    i64   cnt = EOTT->apdx;
    // free tokens
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

    // compilation time
    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);

    printf("-compiled %s into %s in ", lddf, out->outn);
    COL(GRN);
    printf("%.5fs\n", dt);
    COL(DEF);

    return out;
}

void cmperr(imut char * err, tkn * arw, tkn * cmpl){
    // flush compilation messages
    printf("\n\n");

    // common prefix
    fprintf(stderr, RED);
    fprintf(stderr, "[ERROR]");
    fprintf(stderr, DEF);

    fprintf(stderr, " %s", err);
    // print arrow
    if(arw){
        fprintf(stderr, " at %ld:%ld:\n", arw->line + 1, arw->coll + 1);
        fprintf(stderr, BLU);

        char stt[csz];
        sprintf(stt, "\n\t%ld | ", arw->line + 1);

        fprintf(stderr, "%s%s\n\t", stt, code.arr[arw->line]);
        fprintf(stderr, DEF);
        fflush(stderr);

        fprintf(stderr, RED);
        for(u16 i = 0; i < arw->coll + strlen(stt) - 2; i++){
            if(i > 3) fprintf(stderr, "~");
            else fprintf(stderr, " ");
        }
        fflush(stderr);

        fprintf(stderr, "^\n");
        fprintf(stderr, DEF);

    // just end it
    } else fprintf(stderr, "\n");

    if(cmpl != nil){
        fprintf(stderr, cmpl->vall.str);

        fprintf(stderr, " at %ld:%ld:\n",
        cmpl->line + 1, cmpl->coll + 1);

        char stt[csz];
        sprintf(stt, "\n\t%ld | ", cmpl->line + 1);

        fprintf(stderr, BLU);
        fprintf(stderr, "%s%s\n\t", stt, code.arr[cmpl->line]);
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