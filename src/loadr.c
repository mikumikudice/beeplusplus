#ifndef BIC
    #include "bic.h"
#endif

// max size per line
u64 csz = LSIZE;

stra load(FILE * fptr){
    stra out = {};
    out.lgrst = 0;
    out.arr = malloc(sizeof(char *));

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

    u64 lcmt[csz << 2][2]; // keep track of where the last comment were opened
    u64 lscp[csz << 2][2]; // keep track of where the last scope were opened
    u64 lpar[csz << 2][2]; // keep track of where the last parentheses were opened

    char last = 0;

    // init first line
    out.arr[out.len] = malloc(LSIZE);

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
            lcmt[clvl][0] = out.len;
            lcmt[clvl][1] = i - 1;
            clvl++;
        // closing comment symbol
        } else if(c == '/' and last == '*'){
            clvl--;
            if(clvl == 0) iscmt = F;
            // number of closing symbols is greater than opening ones
            else if(clvl < 0){
                tkn arrow = {
                    .line = out.len,
                    .coln = i
                };
                out.arr[out.len][i++] = c;
                out.arr[out.len][i] = '\0';
                cmperr(UNEXPCT, &arrow, nil);
            }
            continue;
        }

        // grow line
        if(i >= csz) {
            csz += LSIZE / 2;
            out.arr[out.len] =
            realloc(out.arr[out.len], (csz) * sizeof(char *));

            if(!out.arr[out.len]) cmperr(REALERR, nil, nil);

            // first realloc
            if(csz == LSIZE * 1.5){
                assert(out.arr[out.len] == nil, UNEXPCT);
                tkn arrw = {
                    .line  = out.len,
                    .coln  = i,
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
                out.arr[out.len][i++] = c;
                if(err) goto skip;

                // when coming back to check symbol, skip it
                if(last != c){
                    // take care of parenthesis and scopes
                    if(c == '{' and !isstr){
                        isscp = T;
                        lscp[plvl][0] = out.len;
                        lscp[plvl][1] = i - 1;
                        slvl++;
                    } else if(c == '}' and !isstr){
                        slvl--;
                        if(!slvl) isscp = F;
                        else if(slvl < 0){
                            arw = (tkn){.line = out.len, .coln = i - 1};
                            err = T;
                        }
                    } else if(c == '(' and !isstr){
                        ispar = T;
                        lpar[plvl][0] = out.len;
                        lpar[plvl][1] = i - 1;
                        plvl++;
                    } else if(c == ')' and !isstr){
                        plvl--;
                        if(!plvl) ispar = F;
                        else if(plvl < 0){
                            arw = (tkn){.line = out.len, .coln = i - 1};
                            err = T;
                        }
                    }
                }
            } else if(i > 0){
                strcpy(out.arr[out.len], "");
                i = 0;
            }
            skip:
            last = c;
        // reached end of line
        } else {
            out.arr[out.len][i] = '\0';
            u64 thisl = strlen(out.arr[out.len]);

            // update largest string length
            if(out.lgrst < thisl) out.lgrst = thisl;

            // catch the error at the end of the line
            if(err) cmperr(UNEXPCT, &arw, nil);

            if(!iscmt){
                // check it here to avoid
                // skipping the last char
                if(c == EOF){
                    out.len++;
                    break;
                }
            // end of line, end of comment
            } else if(isslc) iscmt = F;

            i = 0;
            // next line
            out.arr = realloc(out.arr, (out.len + 2) * sizeof(char *));
            if(!out.arr) cmperr(REALERR, nil, nil);

            // alloc next line
            out.len++;

            if(iscmt and !isslc){
                out.arr[out.len] = malloc(2);
                strcpy(out.arr[out.len], "");
            } else {
                // resset length
                csz = LSIZE;
                out.arr[out.len] = malloc(LSIZE);
            }
        }
    }
    fclose(fptr);

    // check for errors
    if(iscmt){
        tkn tmp = {
            .line = out.len - 1,
            .coln = strlen(out.arr[out.len - 1]) - 1
        };
        tkn cmp = {
            .line = lcmt[clvl - 1][0],
            .coln = lcmt[clvl - 1][1],
            .vall.str = "previously started here"
        };
        cmperr(UNCCMMT, &tmp, &cmp);
    }
    else if(isscp){
        tkn tmp = {
            .line = out.len - 1,
            .coln = strlen(out.arr[out.len - 1]) - 1
        };
        tkn cmp = {
            .line = lscp[slvl - 1][0],
            .coln = lscp[slvl - 1][1],
            .vall.str = "previously started here"
        };
        cmperr(UNCBRCK, &tmp, &cmp);
    } else if(ispar){
        tkn tmp = {
            .line = out.len - 1,
            .coln = strlen(out.arr[out.len - 1]) - 1
        };
        tkn cmp = {
            .line = lpar[plvl - 1][0],
            .coln = lpar[plvl - 1][1],
            .vall.str = "previously started here"
        };
        printf("%d %d %d\n", plvl, plvl - 1, lpar[plvl - 1][1]);
        cmperr(UNCPARN, &tmp, &cmp);
    }

    return out;
}