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

    if(str[chr - 1] == '*'
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

// using rookieslab method
u64 upow(u64 b, u64 p){
    u64 out = 1;
    while(p > 0){
        if(p & 1){
            out = (out * b);
        }
        b = (b * b);
        p >>= 2;
    }
    return out;
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

tkn * lexit(){
    // primitive ast output
    tkn * out = malloc(sizeof(tkn));
    tkn **tok = &out;

    bool iscmt = F; // is the current sentence a comment?
    bool isspl = F; // the comment in question is single-lined?
    bool isscp = F; // is the current content within a scope?
    bool ispar = F; // is the current content within parentheses?

    i64  clvl  = 0; // current comment nesting level
    i64  slvl  = 0; // current scope level
    i64  plvl  = 0; // current parentheses level
    
    // potential stack overflow?
    u64  lcmt[csz * code.len][2]; // keep track of where the last comment were oppened
    u64  lscp[csz * code.len][2]; // keep track of where the last scope were oppened
    u64  lpar[csz * code.len][2]; // keep track of where the last parentheses were oppened

    i64 last = -1;

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
            // enter multiline comment
            if(src[c] == '/' and src[(c + 1) % lsz] == '*' and !isstr){
                // store index of the first comment symbol
                if(!iscmt){
                    lcmt[clvl][0] = l;
                    lcmt[clvl][0] = c;
                }
                clvl++;
                iscmt = T;
                c += 2;
                if(c > lsz) continue;
            }

            if(!isspl)
            // exit multiline comment
            if(src[c] == '*' and src[(c + 1) % lsz] == '/' and !isstr){
                clvl--;
                if(!clvl) iscmt = F;
                else if(clvl < 0){
                    tkn this = {.line = l, .coll = c};
                    cmperr(UNEXPCT, &this, nil);
                }

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

            // when coming back to check symbol, skip it
            if(last != c){
                // take care of parenthesis and scopes
                if(src[c] == '{' and !isstr){
                    isscp = T;
                    lscp[plvl][0] = l;
                    lscp[plvl][1] = c;
                    slvl++;
                }
                else if(src[c] == '}' and !isstr){
                    slvl--;
                    if(!slvl) isscp = F;
                    else if(plvl < 0){
                        tkn this = {.line = l, .coll = c};
                        cmperr(UNEXPCT, &this, nil);
                    }
                }

                else if(src[c] == '(' and !isstr){
                    ispar = T;
                    lpar[plvl][0] = l;
                    lpar[plvl][1] = c;
                    plvl++;
                }
                else if(src[c] == ')' and !isstr){
                    plvl--;
                    if(!plvl) ispar = F;
                    else if(plvl < 0){
                        tkn this = {.line = l, .coll = c};
                        cmperr(UNEXPCT, &this, nil);
                    }
                }
            }
            last = c;

            // it's a token letter, just append
            if(validn(src[c])) ctkn[ctp++] = src[c];
            // do something now
            else {
                tkn this;
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
                            this.vall.str = strtohex(dummy);
                        // it's a string
                        } else {
                            // store the string literals at a
                            // table to be put in the head of
                            // the object file and in the tkn
                            // just store the pointer to it.
                            this.vall.str = strtoptr(dummy);
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
                    if(idx != -1){
                        this.type = KEYWORD;
                        this.vall.num = idx;
                    } else {
                        bool ish = F, isb = F, iso = F, isn = F;
                        bool hashexpr, hasbinpr, hasoctpr;
                        u64 msd_h = 0, msd_b = 0, msd_o = 0;
                        u64 size = strlen(ctkn);

                        hashexpr = startswith(ctkn, "0x");
                        hasbinpr = startswith(ctkn, "0b");
                        hasoctpr = startswith(ctkn, "0");

                        // validate the literals

                        // as hexadecimal
                        if(hashexpr)
                        for(u16 chr = 2; chr < size; chr++){
                            ish = ishexc(ctkn[chr]);
                            if(!ish) break;
                            if(!msd_h and ctkn[chr] > '0')
                            msd_h = chr;
                        }
                        // as binary
                        else if(hasbinpr)
                        for(u16 chr = 2; chr < size; chr++){
                            isb = ctkn[chr] == '0' or ctkn[chr] == '1';
                            if(!isb) break;
                            if(!msd_b and ctkn[chr] > '0')
                            msd_b = chr;
                        }
                        // as octal
                        else if(hasoctpr)
                        for(u16 chr = 2; chr < size; chr++){
                            iso = ctkn[chr] >= '0' or ctkn[chr] <= '7';
                            if(!iso) break;
                            if(!msd_o and ctkn[chr] > '0')
                            msd_o = chr;
                        }
                        // as decimal
                        else
                        for(u16 chr = 0; chr < size; chr++){
                            isn = isnumc(ctkn[chr]);
                            if(!isn) break;
                        }

                        // hex literal
                        if(hashexpr){
                            if(ish){
                                this.type = LITERAL;
                                this.vall.str = malloc(strlen(ctkn) + 1);
                                strcpy(this.vall.str, ctkn);
                            // invalid pattern
                            } else
                                cmperr(UNEXPCT, &this, nil);
                        }
                        // bin literal
                        else if(hasbinpr){
                            if(isb){
                                this.type = LITERAL;
                                u64 val = 0,
                                len = strlen(ctkn) - 2;

                                for(u64 d = 0; d < len; d++){
                                    val += upow(2, len - d - 1);
                                }

                                u64 tmp = upow(2, len - 1);
                                u16 cnt = 0;

                                while(tmp > 0) tmp /= 16, cnt++;
                                this.vall.str = malloc(cnt + 1);

                                // store all numeric values as hexadecimal
                                sprintf(this.vall.str, "%lx", val);
                            // invalid pattern
                            } else
                            cmperr(UNEXPCT, &this, nil);
                        }
                        // oct literal
                        else if(hasoctpr){
                            if(iso){
                                this.type = LITERAL;
                                u64 len = upow(msd_h, 8);
                                len *= src[msd_h] - 48;
                                u64 tmp = len;

                                u16 cnt = 0;
                                while(tmp > 0) tmp /= 16, cnt++;

                                u64 val;
                                sscanf(ctkn, "%lo", &val);

                                // store all numeric values as hexadecimal
                                this.vall.str = malloc(cnt + 1);
                                sprintf(this.vall.str, "%lx", val);
                            // invalid pattern
                            } else
                            cmperr(UNEXPCT, &this, nil);
                        } else {
                            // decimal literal
                            if(isn)
                                this.type = LITERAL;
                            // indexer
                            else
                                this.type = INDEXER;

                            this.vall.str = malloc(strlen(ctkn) + 1);
                            strcpy(this.vall.str, ctkn);
                        }
                    }
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
                                this.vall.num = s;
                                this.type = LSYMBOL;
                                this.apdx = 1;

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
                            this.vall.num = s;
                            this.type = LSYMBOL;
                            this.apdx = 0;

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
                            this.vall.num = o;
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

                // "out" points to the this' next token,
                // the old out's next now points to this

                // old_out.next = this | new_out = this.next

                memcpy((*tok), &this, sizeof(tkn));
                tok = &(*tok)->next;

                if(
                    !(l == code.len - 1 and col >= lsz)
                // only alloc if it's not the last token
                ) *tok = malloc(sizeof(tkn));
            }
        }
        if(isstr){
            tkn arrw = {
                .line  = l,
                .coll  = strlen(src) - 1
            };
            cmperr(UNCLSTR, &arrw, nil);
        }
        free(src);
    }

    if(iscmt){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lcmt[clvl - 1][0],
            .coll = lcmt[clvl - 1][1]
        };
        cmp.vall.str = malloc(24);

        strcpy(cmp.vall.str, "previously started here");
        cmperr(UNCCMMT, &tmp, &cmp);
    }
    else if(isscp){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lscp[slvl - 1][0],
            .coll = lscp[slvl - 1][1]
        };
        cmp.vall.str = malloc(24);

        strcpy(cmp.vall.str, "previously started here");
        cmperr(UNCBRCK, &tmp, &cmp);
    }
    else if(ispar){
        tkn tmp = {
            .line = code.len - 1,
            .coll = strlen(code.arr[code.len - 1]) - 1
        };
        tkn cmp = {
            .line = lpar[plvl - 1][0],
            .coll = lpar[plvl - 1][1]
        };
        cmp.vall.str = malloc(24);

        strcpy(cmp.vall.str, "previously started here");
        cmperr(UNCPARN, &tmp, &cmp);
    }

    if(*tok != nil)
        (*tok)->next = &EOFT;
    else
        cmperr(
        "a really unexpected error occured"\
        ". please report [r101]", nil, nil);

    // return out, that points to the first token,
    // and not tok, that points to the last token
    return out;
}