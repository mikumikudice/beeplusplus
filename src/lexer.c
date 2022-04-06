#ifndef BIC
    #include "bic.h"
#endif

// header-located string array
stra sarr;

// checks if the given character is a valid decimal number digit
bool isnumc(char chr){
    return isdigit(chr);
}

// checks if the given character is a valid hexadecimal number digit
bool ishexc(char chr){
    return isxdigit(chr);
}

// checks if the given character is a valid namespace one
bool validn(char chr){
    return (('0' <= chr and chr <= '9') or
            ('A' <= chr and chr <= 'Z') or
            ('a' <= chr and chr <= 'z') or
    chr == '_');
}

// checks if the given string matches all chars according to the given func
bool matchs(char * str, bool(*func)(char)){
    bool valid = T;
    for(u64 c = 0; c < strlen(str); c++){
        if(valid) valid = func(str[c]);
        else break;
    }
    return valid;
}

// checks if the char in str is scaped (seeks for backslashes)
bool isscpd(char * str, u64 chr){
    if(chr < 0) return F;

    if(str[chr - 1] == '*'
    and !isscpd(str, chr - 1)) return T;
    else return F;
}

// returns the index of the keyword if str is one, otherwise returns -1
i16 iskeyw(char * str){
    u16 len = arrlen(KEYWORDS);
    for(u16 kw = 0; kw < len; kw++){
        if(!strcmp(str, KEYWORDS[kw])) return kw;
    }
    return -1;
}

// power function
u64 upow(u64 b, u64 p){
    // using rookieslab method
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

// returns the token value as a string
char * get_tokval(tkn * tok){
    switch(tok->type){
        case KEYWORD:
            return KEYWORDS[tok->vall.num];
        case INDEXER:
            return tok->vall.str;
        case LITERAL:            
            return "LITERAL";
        case LSYMBOL:
            if(tok->vall.num >= arrlen(SYMBOLS)) return "OOB";
            if(tok->apdx == 0) return SYMBOLS[tok->vall.num].s;
            else return SYMBOLS[tok->vall.num].e;
        case OPERATOR:
            return OPERATORS[tok->vall.num];
        default:
            return "NONE";
    }
}

// converts the string path to a hexadecimal value
u32 strtohex(char * data){
    u32 out = 0x00000000;
    u16 len = strlen(data);
    assert(len <= 4, nil);

    for(u16 d = 4; d > 0; d--){
        if(d <= len){
            out |= data[len - d] << d * 8;
        }
    }
    return out;
}

// To avoid use a lot of memory duplication, making one instance
// for each string literal, store each occurrence in a table and
// return the index of it. It also helps with the code gen step,
// by placing the values in the .rodata segment of the nasm file
u64 strtoptr(char * str){
    // init array if its nil
    if(!sarr.arr) sarr.arr = alloc(sizeof(char *));
    for(u64 s = 0; s < sarr.len; s++){
        // return the known string value's index
        if(!strcmp(str, sarr.arr[s])) return s;
    }
    // push the new string
    stra_push(sarr, str);
    return sarr.len - 1;
}

// free all global string instances
void free_str(){
    for(u64 s = 0; s < sarr.len; s++){
        free(sarr.arr[s]);
    }
}

tkn * lexit(){
    // primitive ast output
    tkn * out;
    tkn **lst = &EOTT; // the address of the last token pointer

    // set up the chain loop
    EOTT->apdx = 0;    // the count of tokens

    // current line
    for(u64 l = 0; l < code.len; l++){
        u64 lsz = strlen(code.arr[l]); // line length

        // skip comment lines
        if(lsz == 0) continue;

        char * src = alloc(lsz + 1);
        strcpy(src, code.arr[l]);

        bool isstr = F; // is the current sentence a string?
        i64  ctc   = 0; // current token's character
        char ctkn[lsz];

        for(u64 c = 0; c < lsz; c++){
            // it's a token letter, so just append it
            if(validn(src[c])){
                ctkn[ctc++] = src[c];
            }
            // it's not, so it's a especial character or a space
            else {
                tkn this; // the next token

                this.type = UNKNOWN;
                this.coln = ctc > 0 ? c - 1 : c;
                this.line = l;
                this.apdx = 0;

                // if we're entering a string literal
                // path just continue appending it
                if(src[c] == metachar[0].val[0]
                or src[c] == metachar[1].val[0]){
                    bool issc = isscpd(src, c);
                    
                    // in and out of string
                    if(!issc) {
                        isstr = !isstr;
                        if(isstr) continue;
                    }
                    if(!isstr){
                        this.type = LITERAL;

                        char * dummy = alloc(strlen(ctkn) + 1);
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
                            this.vall.num = strtohex(dummy);
                            this.apdx = DWCHAR;

                            free(dummy);

                        // it's a string
                        } else {
                            // store the string literals at a
                            // table to be put in the head of
                            // the object file and in the tkn
                            // just store the pointer to it.
                            u64 indx = strtoptr(dummy);
                            if(sarr.arr[indx] != dummy) free(dummy);

                            this.vall.num = indx;
                            this.apdx = STRING;
                        }
                        ctc = 0;
                        goto finish;
                    }
                    // keep storing
                    else if(issc) ctkn[ctc++] = src[c];
                    continue;
                }
                // keep storing
                else if(isstr){
                    ctkn[ctc++] = src[c];
                    continue;
                }
                // the read token is a keyword, index or literal
                else if(ctc > 0){
                    // first close the buffer
                    ctkn[ctc] = '\0';
                    // resset the current token
                    // pointer to the beginning
                    ctc = 0;

                    // check if it's a keyword
                    i16 idx = iskeyw(ctkn);

                    // keyword
                    if(idx != -1){
                        this.type = KEYWORD;
                        this.vall.num = idx;
                    } else {
                        // parse as number literals
                        bool ish = F, isb = F, iso = F, isn = F;
                        bool hashexpr, hasbinpr, hasoctpr;
                        
                        // pointers to the start of the number
                        i64 h_stt = -1, b_stt = -1, o_stt = -1, d_stt = -1;
                        u64 size = strlen(ctkn);

                        hashexpr = startswith(ctkn, "0x");
                        hasbinpr = startswith(ctkn, "0b");
                        hasoctpr = startswith(ctkn, "0o");

                        // validate the literals

                        // as hexadecimal
                        if(hashexpr)
                        for(u16 chr = 2; chr < size; chr++){
                            ish = ishexc(ctkn[chr]);
                            if(!ish) break;

                            if(h_stt == -1 and ctkn[chr] > '0')
                            h_stt = chr;
                        }
                        // as binary
                        else if(hasbinpr)
                        for(u16 chr = 2; chr < size; chr++){
                            isb = ctkn[chr] == '0' or ctkn[chr] == '1';
                            if(!isb) break;

                            if(b_stt == -1 and ctkn[chr] > '0')
                            b_stt = chr;
                        }
                        // as octal
                        else if(hasoctpr)
                        for(u16 chr = 2; chr < size; chr++){
                            iso = ctkn[chr] >= '0' or ctkn[chr] <= '7';
                            if(!iso) break;

                            if(o_stt == -1 and ctkn[chr] > '0')
                            o_stt = chr;
                        }
                        // as decimal
                        else
                        for(u16 chr = 0; chr < size; chr++){
                            isn = isnumc(ctkn[chr]);
                            if(!isn) break;

                            if(d_stt == -1 and ctkn[chr] > '0')
                            d_stt = chr;
                        }

                        // hex literal
                        if(hashexpr){
                            if(ish){
                                this.type = LITERAL;
                                this.apdx = NUMBER;
                                sscanf(ctkn, "%x", &this.vall.num);
                            // invalid pattern
                            } else
                                cmperr(UNEXPCT, &this, nil);
                        }
                        // bin literal
                        else if(hasbinpr){
                            if(isb){
                                this.type = LITERAL;
                                this.apdx = NUMBER;
                                this.vall.num = 0;

                                u64 len = strlen(ctkn) - 2;
                                for(u64 d = len; d > 1; d--){
                                    if(ctkn[d] == '1')
                                        this.vall.num += upow(2, d - 1);
                                }
                            // invalid pattern
                            } else
                                cmperr(UNEXPCT, &this, nil);
                        }
                        // oct literal
                        else if(hasoctpr){
                            if(iso){
                                this.type = LITERAL;
                                this.apdx = NUMBER;
                                sscanf(ctkn + 2, "%lo", &this.vall.num);
                            // invalid pattern
                            } else
                                cmperr(UNEXPCT, &this, nil);
                        } else {
                            // decimal literal
                            if(isn){
                                this.type = LITERAL;
                                sscanf(ctkn, "%d", &this.vall.num);
                            // indexer
                            } else if(!isnumc(ctkn[0])){
                                this.type = INDEXER;
                                this.vall.str = alloc(strlen(ctkn) + 1);
                                strcpy(this.vall.str, ctkn);

                            // invalid indexer name
                            } else cmperr(INVALID, &this, nil);
                        }
                    }
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
                        if(SYMBOLS[s].e){
                            // set token to the same
                            // slice size of the sym
                            char t[alen + 1];
                            // avoid multiple calls
                            u16 len = strlen(SYMBOLS[s].e);

                            // append missing chars
                            for(u16 chr = c; chr < c + len and chr < lsz; chr++){
                                t[chr - c] = src[chr];
                            }
                            t[len] = '\0';

                            if(!strcmp(SYMBOLS[s].e, t)){
                                this.vall.num = s;
                                this.type = LSYMBOL;
                                this.apdx = 1;

                                c += len - 1;
                                break;
                            }
                        }
                        // same of the last statement
                        u16 len = strlen(SYMBOLS[s].s);
                        char t[len + 1];

                        for(u16 chr = c; chr < c + len and chr < lsz; chr++){
                            t[chr - c] = src[chr];
                        }
                        t[len] = '\0';

                        if(!strcmp(SYMBOLS[s].s, t)){
                            this.vall.num = s;
                            this.type = LSYMBOL;
                            this.apdx = 0;

                            c += len - 1;
                            break;
                        }
                    }
                    if(this.type != UNKNOWN) goto finish;

                    u16 mlen = 0; // matched len
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
                        if(!strcmp(OPERATORS[o], t) and len > mlen){
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
                assert(this.type != UNKNOWN, "token not evaluated");

                // store the previous token on the current
                this.last = *lst;

                tkn ** next;
                if(EOTT->apdx == 0) next = &out;
                else next = &(*lst)->next;

                // once the token is already on the chain,
                // move to the current's next token
                *next = alloc(sizeof(tkn));
                memcpy(*next, &this, sizeof(tkn));

                if(EOTT->apdx == 0) EOTT->next = *next;

                if(l == code.len - 1 and c == lsz - 1){
                    // EOTT holds the first token and the first token
                    // holds EOTT, so this chain is circular
                    (*next)->next = EOTT;
                    EOTT->last = *next;
                }
                // the next token is the next of the current, so update lst
                lst = next;

                // the first token holds the count of tokens
                EOTT->apdx++;
            }
        }
        if(isstr){
            tkn arrw = {
                .line  = l,
                .coln  = strlen(src) - 1
            };
            cmperr(UNCLSTR, &arrw, nil);
        }
        free(src);
    }
    // return out, that points to
    // the first and last tokens
    return out;
}