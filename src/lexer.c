#ifndef BIC
    #include "bic.h"
#endif

// header-located string array
charr sarr;

bool isnumc(char chr){
    return isdigit(chr);
}

bool ishexc(char chr){
    return isxdigit(chr);
}

bool validn(char chr){
    return (('0' <= chr and chr <= '9') or
            ('A' <= chr and chr <= 'Z') or
            ('a' <= chr and chr <= 'z') or
    chr == '_');
}

bool matchs(char * str, bool(*func)(char)){
    bool valid = T;
    for(u64 c = 0; c < strlen(str); c++){
        if(valid) valid = func(str[c]);
        else break;
    }
    return valid;
}

bool hassym(char * str){
    u16 len = arrlen(SYMBOLS);
    for(u16 p = 0; p < len; p++){
        if(SYMBOLS[p].s != nil)
        if(endswith(str, SYMBOLS[p].s)) return T;

        if(endswith(str, SYMBOLS[p].e)) return T;
    }
    return F;
}

bool isscpd(char * str, u64 chr){
    if(chr < 0) return F;

    if(str[chr - 1] == '\\'
    and !isscpd(str, chr - 1)) return T;
    else return F;
}

i16 iskeyw(char * str){
    u16 len = arrlen(KEYWORDS);
    for(u16 kw = 0; kw < len; kw++){
        if(!strcmp(str, KEYWORDS[kw])) return kw;
    }
    return -1;
}

char * strtohex(char * data){
    // avoid too many calls
    u16 len = arrlen(metachar);
    for(u16 mc = 0; mc < len; mc++){
        data = strgsub(data, metachar[mc].key, metachar[mc].val);
    }

    // keep it in memory
    char temp[strlen(data) + 1];
    strcpy(temp, data);

    free(data);
    data = malloc(strlen(temp) * 2 + 3);
    sprintf(data, "0x");

    char othr[strlen(temp) * 2 + 3];

    // convert string to hex code
    for(u64 c = 0; c < strlen(temp); c++){
        strcpy(othr, data);
        sprintf(data, "%s%.2x", othr, temp[c]);
    }
    // keep length constant
    while(strlen(data) < 10) data = strpush(data, "0");

    return data;
}

char * strtoptr(char * str){
    if(!sarr.arr) sarr.arr = malloc(sizeof(char *));
    for(u64 s = 0; s < sarr.len; s++){
        if(!strcmp(str, sarr.arr[s])) return sarr.arr[s];
    }
    carr_push(sarr, str);
    return sarr.arr[sarr.len - 1];
}

lexout lexit(){
    // primitive ast output
    lexout out;
    out.tkns = malloc(sizeof(token));
    out.tknc = 0; // just to make sure

    token cidx;     // index of the multiline comment start symbol
    bool iscmt = F;// is the current sentence a comment?
    bool isspl = F; // the comment in question is single-lined?
    u64  clvl  = 0; // current comment nesting level

    // current line
    for(u64 l = 0; l < code.len; l++){
        u64 col = 0;                   // last token's column
        u64 lsz = strlen(code.arr[l]); // line length

        char * src = malloc(lsz + 1);
        strcpy(src, code.arr[l]);

        bool isstr = F; // is the current sentence a string?
        u64  ctp   = 0; // current token's pointer
        char ctkn[lsz];

        for(u64 c = 0; c < lsz; c++){
            printf("%d %c %s\n", c, src[c], src);
            // enter multiline comment
            if(src[c] == '/' and src[(c + 1) % lsz] == '*' and !isstr){
                clvl++;
                // store index of the first comment symbol
                if(!iscmt){
                    cidx.coll = c;
                    cidx.line = l;
                }
                iscmt = T;
                c += 2;
                if(c > lsz) continue;
            }

            if(!isspl)
            // exit multiline comment
            if(src[c] == '*' and src[(c + 1) % lsz] == '/' and !isstr){
                clvl--;
                if(!clvl) iscmt = F;

                c += 2; // skip to the char after the symbol
                if(c >= lsz) continue;
            }

            // single line comment
            if(c + 1 < lsz)
            if(src[c] == '/' and src[c + 1] == '/'){
                iscmt = T;
                isspl = T;
            }
            if(isspl and iscmt)
            if(c == lsz - 1){
                iscmt = F;
                isspl = F;
                continue;
            }
            if(iscmt) continue;

            // it's a token piece, just append
            if(validn(src[c])) ctkn[ctp++] = src[c];
            // do something now
            else {
                token this;

                this.type = UNKNOWN;
                this.coll = col;
                this.line = l;

                // ignore strings
                if(src[c] == metachar[0].val[0]
                or src[c] == metachar[1].val[0]){
                    bool issc = isscpd(src, c);
                    
                    // in and out of string
                    if(!issc) isstr = !isstr;

                    if(!isstr){
                        char * dummy = malloc(strlen(ctkn) + 1);
                        strcpy(dummy, ctkn);

                        u64 len = arrlen(metachar);
                        for(u16 mc = 0; mc < len; mc++){
                            dummy = strgsub(dummy,
                            metachar[mc].key, metachar[mc].val);
                        }
                        // it's a doubleword char
                        if(src[c] == metachar[0].val[0]){
                            // slice it
                            if(strlen(dummy) > 4){
                                char * temp = dummy;
                                dummy = str_sub(dummy, 0, 4);
                                free(temp);
                            }
                            this.vall = strtohex(dummy);
                        // it's a string
                        } else {
                            // store the string literals at a
                            // table to be put in the head of
                            // the object file and in the tkn
                            // just store the pointer to it.
                            this.vall = strtoptr(dummy);
                        }

                        this.type = LITERAL;
                        ctp = 0;
                        goto finish;
                    }
                    // keep storing
                    else if(issc) ctkn[ctp++] = src[c];
                    continue;
                }
                // keep storing
                else if(isstr){
                    ctkn[ctp++] = src[c];
                    continue;
                }
                // keyword, index or literal
                else if(ctp > 0){
                    // just close the buffer
                    ctkn[ctp] = '\0';

                    i16 idx = iskeyw(ctkn);

                    // keyword
                    if(idx != -1)
                        this.type = KEYWORD;
                    else {
                        bool ish = F, isn = F, hashexdef;
                        u64 size = strlen(ctkn);

                        hashexdef = startswith(ctkn, "0x");

                        // validate the literals
                        if(hashexdef)
                        for(u16 chr = 2; chr < size; chr++){
                            ish = ishexc(ctkn[chr]);
                        }
                        else
                        for(u16 chr = 0; chr < size; chr++){
                            isn = isnumc(ctkn[chr]);
                        }

                        // hexadecimal literal
                        if(hashexdef){
                            if(ish)
                            this.type = LITERAL;
                            // invalid pattern
                            else
                            cmperr(UNEXPCT, &this, nil);
                        }
                        // decimal literal
                        else if(isn)
                            this.type = LITERAL;
                        // indexer
                        else this.type = INDEXER;
                    }
                    this.vall = malloc(strlen(ctkn) + 1);
                    strcpy(this.vall, ctkn);

                    // resset the current token
                    // pointer to the beginning
                    ctp = 0;

                    // decrement it if the
                    // current char may be 
                    // a symbol
                    if(src[c] != ' ') c--;
                }
                // symbol or operator
                else if(src[c] != ' '){
                    // just to make sure
                    this.vall = nil;

                    u16 alen = arrlen(SYMBOLS);
                    // compare with the known symbols
                    for(u16 s = 0; s < alen; s++){
                        // not all symbols are in pairs
                        if(SYMBOLS[s].s){
                            // set token to the same
                            // slice size of the sym
                            char t[alen + 1];
                            // avoid multiple calls
                            u16 len = strlen(SYMBOLS[s].s);

                            // append missing chars
                            for(u16 chr = c; chr < c + len and chr < lsz; chr++){
                                t[chr - c] = src[chr];
                            }
                            t[len] = '\0';

                            if(!strcmp(SYMBOLS[s].s, t)){
                                this.vall = malloc(len + 1);
                                strcpy(this.vall, SYMBOLS[s].s);

                                this.type = LSYMBOL;
                                c += len - 1;
                                break;
                            }
                        }
                        // same of the last statement
                        u16 len = strlen(SYMBOLS[s].e);
                        char t[len + 1];

                        for(u16 chr = c; chr < c + len and chr < lsz; chr++){
                            t[chr - c] = src[chr];
                        }
                        t[len] = '\0';

                        if(!strcmp(SYMBOLS[s].e, t)){
                            this.vall = malloc(len + 1);
                            strcpy(this.vall, SYMBOLS[s].e);

                            this.type = LSYMBOL;
                            c += len - 1;
                            break;
                        }
                    }
                    if(this.type != UNKNOWN) goto finish;

                    u16 mlen; // matched len
                    alen = arrlen(OPERATORS);
                    // compare with the known operators
                    for(u16 o = 0; o < alen; o++){
                        // set token to the same
                        // slice size of the sym
                        char t[alen + 1];
                        // avoid multiple calls
                        u16 len = strlen(OPERATORS[o]);

                        // append missing chars
                        for(u16 chr = c; chr < c + len and chr < lsz; chr++){
                            t[chr - c] = src[chr];
                        }
                        t[len] = '\0';

                        // just check, but don't stop. it
                        // may occur some mismatches like
                        // =< being read as =, so keep it
                        // going until the end
                        if(!strcmp(OPERATORS[o], t)){
                            // avoid memory leaks
                            if(this.vall != nil) free(this.vall);

                            this.vall = malloc(len + 1);
                            strcpy(this.vall, OPERATORS[o]);

                            this.type = OPERATOR;
                            mlen = len;
                        }
                    }
                    // move pointer after operator
                    c += mlen - 1;

                // just a space out of a string
                } else continue;

                finish:
                if(this.type != UNKNOWN) col = c + 1;
                else cmperr(UNEXPCT, &this, nil);

                tkn_push(out, this);
                if(!out.tkns) cmperr(REALERR, nil, nil);
            }
        }
        if(isstr){
            token arrw = {
                .line  = l,
                .coll  = strlen(src) - 1
            };
            cmperr(UNCLSTR, &arrw, nil);
        }
        free(src);
    }

    if(iscmt){
        token tmp = (token){nil, 0, code.len - 1,
        strlen(code.arr[code.len - 1]) - 1};
        
        cidx.vall = malloc(24);
        sprintf(cidx.vall, "previously started here");

        cmperr(UNCCMMT, &tmp, &cidx); // TODO: invert after fix
    }

    return out;
}