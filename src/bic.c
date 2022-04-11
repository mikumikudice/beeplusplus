#define LSIZE (64 << 1) // max of bytes read per line

// compiler messages
imut char *PRVONHR = "previously opened here";
imut char *OBS_IFB = "obs: you cannot define an if/elif/else block without curly brackets";
imut char *IPATH_E = "obs: the compiler was expecting a string: the path to the extern file";
imut char *PARAMPT = "tip: if you're trying to define a parameter as pointer, use `fn([foo])`";

// error messages
imut char *REALERR = "an unexpected error occurred";
imut char *UNEXPCT = "unexpected symbol found here";
imut char *UNCLSTR = "unclosed string found here";
imut char *UNCCMMT = "unclosed multiline comment found here";
imut char *UNCBRCK = "unclosed bracket found here";
imut char *UNCPARN = "unclosed parentheses found here";
imut char *ALONEXP = "expression out of context found here";
imut char *CALLERR = "attempt to call an undefined function";
imut char *CALLINV = "attempt to call an invalid symbol as function";
imut char *TOOFEWC = "too few arguments in the statement";
imut char *TOOMUCH = "too much arguments in the statement";
imut char *MULTIDF = "attempt to define multiple times the same namespace";
imut char *MDINSTT = "attempt to set multiple definitions in a single statement";
imut char *LITRIDX = "attempt to use a literal as namespace";
imut char *KWRDIDX = "attempt to use a keyword as namespace";
imut char *KWRDVAL = "attempt to use a invalid statement as value";
imut char *EXPCTDP = "expected a parentheses at the end of this expression";
imut char *EXPCTEX = "expected expression at this point";
imut char *EXPCTBD = "expected body definition at this point";
imut char *EXPVRHD = "expected valid right-hand value";
imut char *EXPVHND = "expected valid evaluatable value";
imut char *EXPPPAR = "expected a closing square bracket after parameter as pointer definition";
imut char *DEFWOEQ = "you cannot assign a variable with shorthand operators while defining it";
imut char *NOTASGN = "this is not a valid assignment operator";
imut char *MSMATCH = "mismatch of number of assignators and assignateds in expression";
imut char *NOTERMN = "no terminator at the end of the expression";
imut char *NOPTRAR = "no arithmetic is allowed on pointers";
imut char *NOTFLKW = "this keyword is not callable";
imut char *INVALID = "invalid namespace name";
imut char *INVLDIX = "invalid indexing value";
imut char *INVLDAC = "invalid field access on this namespace";

// string array of each line of code
stra code;

cout *comp(FILE * fptr, char * lddf, char * mode){
    // output
    cout *out = alloc(sizeof(out));

    // TODO: parse the compiler config comments
    
    // set the default output name
    char  *dummy = alloc(strlen(lddf) + 1);
    strcpy(dummy, lddf);
    out->outn  = strgsub(dummy, ".bi", "");

    // compiling timer
    clock_t crnt, oldt = clock();
    f32 dt;

    COL(GRN);
    printf("+loading code  ...");
    COL(DEF);

    // load code
    code = load(fptr);

    // time to load the file
    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);
    
    COL(GRN);
    printf(" done %sin %s%.5fs\n", DEF, BLU, dt);
    COL(DEF);

    COL(GRN);
    printf("+lexing code   ...");
    COL(DEF);

    tkn *tkns = lexit();

    // time to lex the file
    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);

    COL(GRN);
    printf(" done %sin %s%.5fs\n", DEF, BLU, dt);
    COL(DEF);

    COL(GRN);
    printf("+parsing code  ...");
    COL(DEF);

    cmod cmd;
    if(!strcmp(mode, "check")) cmd = CHECK;
    else if (!strcmp(mode, "debug")) cmd = DEBUG;
    else cmd = BUILD;

    // basic ast
    node *bast = parse(tkns, cmd);
    if(cmd == CHECK) goto skip_cmp;

    // time to parse the file
    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);

    COL(GRN);
    printf(" done %sin %s%.5fs\n", DEF, BLU, dt);
    COL(DEF);

    // TODO: final ast gen
    if(cmd == DEBUG) goto skip_opt;
    // TODO: optimizations
    skip_opt:
    // TODO: NASM gen

    skip_cmp:
    printf("\n");
    free_node(bast, F);

    tkn *tok,  *old = nil;
    i64  cnt = EOTT->apdx;
    // free tokens
    for(tok = tkns; cnt > 0; tok = tok->next, cnt--){
        // only free string values
        if(tok->type == INDEXER) free(tok->vall.str);
        if(old) free(old);
        old = tok;
    }
    free(old);
    free_str();

    // total compilation time
    crnt = clock();
    dt = ((double)(crnt - oldt)) / ((double)CLOCKS_PER_SEC);

    printf("-compiled %s%s%s into %s%s%s in ", GRN, lddf, DEF, GRN, out->outn, DEF);
    COL(GRN);
    printf("%.5fs\n", dt);
    COL(DEF);

    return out;
}

void free_node(node * n, bool silent){
    char *pout;
    cnst bool ctrl = F; 
    if(!n->is_parent){
        if(!silent){
            pout = nodet_to_str(n);
            printf("%s ", pout);
            free(pout);
        }
        free(n);
    } else {
        ctrl = 0;
        node *tmp = nil, *ths = n->stt, *end = n->end;

        if(!silent){
            pout = nodet_to_str(n);
            printf("%s\n", pout);
            free(pout);
        }
        free(n);
        while(T){
            assert(ths != nil, nodet_to_str(n));

            tmp = ths;
            ths = tmp->next;

            // it's the end of the branch
            if(tmp == end){
                free_node(tmp, silent);
                if(!silent && !ctrl){
                    printf("\n\n");
                    ctrl = T;
                }
                break;
            } else free_node(tmp, silent);
        }
    }
}

void cmperr(imut char *err, tkn * arw, tkn * cmpl){
    // flush compilation messages
    printf("\n\n");

    // common prefix
    fprintf(stderr, RED);
    fprintf(stderr, "[ERROR]");
    fprintf(stderr, DEF);

    fprintf(stderr, " %s", err);
    // print arrow
    if(arw){
        fprintf(stderr, " at %ld:%ld:\n", arw->line + 1, arw->coln + 1);
        fprintf(stderr, BLU);

        char stt[code.lgrst];
        sprintf(stt, "\n\t%ld | ", arw->line + 1);

        fprintf(stderr, "%s%s\n\t", stt, code.arr[arw->line]);
        fprintf(stderr, DEF);
        fflush(stderr);

        fprintf(stderr, RED);
        for(u16 i = 0; i < arw->coln + strlen(stt) - 2; i++){
            if(i > 3) fprintf(stderr, "~");
            else fprintf(stderr, " ");
        }
        fflush(stderr);

        fprintf(stderr, "^\n");
        fprintf(stderr, DEF);

    // just end it
    } else fprintf(stderr, "\n");

    if(cmpl != nil){
        fprintf(stderr, "\n%s", cmpl->vall.str);

        if(cmpl->apdx == 0){
            fprintf(stderr, " at %ld:%ld:\n",
            cmpl->line + 1, cmpl->coln + 1);

            char stt[code.lgrst];
            sprintf(stt, "\n\t%ld | ", cmpl->line + 1);

            fprintf(stderr, BLU);
            fprintf(stderr, "%s%s\n\t", stt, code.arr[cmpl->line]);
            fprintf(stderr, DEF);
            fflush(stderr);

            fprintf(stderr, BLU);
            for(u16 i = 0; i < cmpl->coln + 4; i++){
                if(i > 3) fprintf(stderr, "~");
                else fprintf(stderr, " ");
            }
            fflush(stderr);

            fprintf(stderr, "^\n");
            fprintf(stderr, DEF);
        } else fprintf(stderr, "\n");
    }
    fprintf(stderr, "\nthe compilation failed.\nplease press any key to exit...");
    scanf("nothing");
    exit(-1);
}

void wrning(imut char *wrn, tkn * arw, tkn * cmpl){
    COL(YEL);
    printf("[WARNING]");
    COL(DEF);

    printf(" at %ld:%ld\n\t%s\n",
    arw->line, arw->coln, wrn);
}