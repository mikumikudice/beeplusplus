#ifndef BIC
    #include "bic.h"
#endif

// checks if token is the given symbol
#define eq_sym(tok, sym, side)\
(tok->type == LSYMBOL and tok->vall.num == sym and tok->apdx == side)

// checks if token is the given operator
#define eq_opr(tok, i_idx, f_idx)\
(tok->type == OPERATOR and tok->vall.num >= i_idx and tok->vall.num <= f_idx)

#define eq_opr_range(tok, t)\
(tok->type == OPERATOR and tok->vall.num >= t[0] and tok->vall.num <= t[1])

#define eq_kwd(tok, t)\
(tok->type == KEYWORD and tok->vall.num >= t[0] and tok->vall.num <= t[1])

// code path rules

astt * define_r(tkn * c){
    astt * path = malloc(sizeof(astt));
    node * pntr = malloc(sizeof(node));

    // basic metadata
    path->strt = c;
    path->pstt = pntr;
    pntr->vall = DEFINE;
    pntr->itsf = c->vall;

    // definition has attribution
    if(c->next->next->type == OPERATOR){
        if(!eq_opr(c->next->next, 0, 0))
            cmperr(DEFWOEQ, c->next->next, nil);

        // a definition is an keyword and an assignment
        // so evaluate the last part and append the kwd
        astt * asg = assign_r(c->next->next, F);

        path->endo = asg->endo;
        pntr->next = asg->pstt;
        path->pend = asg->pend;

        free(asg);
    // only namespace
    } else {
        path->endo = c->next;

        pntr->next = malloc(sizeof(node));
        pntr = pntr->next;
        pntr->vall = INDEXER;
        pntr->itsf = c->next->vall;
        path->pend = pntr;
    }
    return path;
}

// assignment hotpath's rule
astt * assign_r(tkn * c, bool prnd){
    // validate operator
    if(!eq_opr_range(c, asgn))
        cmperr(NOTASGN, c, nil);

    astt * path = malloc(sizeof(astt));
    node * pntr = malloc(sizeof(node));

    path->pstt = pntr;
    path->pstt->itsf.num = 0xA55;

    tkn * strt = c->last;

    // Type A: <type> foo = bar [, spm = egg]*

    // validate lefthand
    if(eq_sym(strt->last, SYM_CLN, 0)
    or eq_sym(strt->last, SYM_COM, 0)
    or eq_sym(strt->last, SYM_SQR, 0)
    or eq_kwd(strt->last, ldef)){
        // the last token before the assigment is a square
        // bracket, a comma, a semicolon or a keyword.
        path->strt = strt;
        pntr->vall = ASSIGN;
        pntr->itsf = strt->vall;

        // operator itself
        pntr->next = malloc(sizeof(node));
        pntr = pntr->next;
        pntr->vall = OPERATOR;
        pntr->itsf = c->vall;

    // invalid position for an assigment
    } else cmperr(
        "this assigment is"
        " at an invalid position", strt, nil);

    // validate the righthand
    if(c->next->type == INDEXER or c->next->type == LITERAL){
        astt *rhnd;
        tkn  *after = c->next->next;

        // check for expressions
        if(after->type == OPERATOR){
            // invalid operators
            if(after->vall.num == 29 or after->vall.num == 31)
                cmperr(UNEXPCT, after, nil);

            rhnd = exprss_r(after, F);
            pntr->next = rhnd->pstt;
            pntr = rhnd->pend;

            // foo = bar + bazz, egg = bar + spam, ...
            // ................^
            //                 you are here

            // if this is not the end of the path, repeat the
            // process recursively until someone returns
            tkn * prox = nil;
            if(eq_sym(rhnd->endo, SYM_COM, 0)){
                while(eq_sym(rhnd->endo->next, SYM_COM, 0)){
                    // prox is the oper of the fallowing exp
                    if(!prox){
                        prox = rhnd->endo->next->next->next;
                        free(rhnd);
                    } else
                        prox = rhnd->endo->next->next->next;

                    // evaluate it
                    rhnd = exprss_r(prox, F);

                    // append it
                    pntr->next = rhnd->pstt;
                    pntr = rhnd->pend;

                    free(rhnd);
                }
            }

        // another sentence
        } else if(eq_sym(after, SYM_COM, 0)){

            rhnd = assign_r(after->next, F);
            // append to the end of the chain
            pntr->next = rhnd->pstt;
            pntr = rhnd->pend;
            
            // update the fallowing token
            after = rhnd->endo->next;
            free(rhnd);

        // just a indexer or a literal
        } else if(eq_sym(after, SYM_CLN, 0)){
            path->endo = after->last;
            // append last node
            pntr->next = malloc(sizeof(node));
            pntr = pntr->next;
            pntr->vall = after->last->type;
            pntr->itsf = after->last->vall;

            goto finish;

        } else cmperr(UNEXPCT, after, nil);

        // finish with metadata
        path->endo = rhnd->endo;
        free(rhnd);

        finish:
        path->pend = pntr;
        return path;

    // the righthand is an expression
    } else if(eq_sym(c->next, SYM_PAR, 0)){
        tkn * after = c->next->next;
        
        // an encapsulated signed value
        if(after->type == OPERATOR){
            astt * exp = exprss_r(after, T);
            pntr->next = exp->pstt;
            pntr = exp->pend;

            path->endo = exp->endo;
            free(exp);

        // it's really an expression
        } else if(after->next->type == OPERATOR){
            astt * exp = exprss_r(after->next, T);
            pntr->next = exp->pstt;
            pntr = exp->pend;

            path->endo = exp->endo;
            free(exp);

        // just a value surrounded by parentheses
        } else {
            pntr->next = malloc(sizeof(node));
            pntr->next->vall = after->type;
            pntr->next->itsf = after->vall;

            path->endo = after;
        }
        // finish with metadata
        path->pend = pntr;

        
        return path;
    }

    tkn *frst = c->last, * last = c->next;
    u64  lcnt = 0, rcnt = 0;
    bool lst_is_cm = T;

    assert(F, nil);

    // Tybe B: <type> foo [, bar]* = spm [, egg]*

    // togle between indexer and semicolon
    // until you find the starting point
    for(;!eq_sym(frst, SYM_CLN, 0) and !eq_sym(strt->last, SYM_SQR, 0);
    frst = frst->last){
        // indexer
        if(frst->type == INDEXER){
            // is this token expected?
            if(lst_is_cm){
                lst_is_cm = F;
                lcnt++;

                pntr->next = malloc(sizeof(node));
                pntr = pntr->next;
                pntr->vall = INDEXER;

                continue;
            } else cmperr(UNEXPCT, frst, nil);

        // comma
        } else if(eq_sym(frst, SYM_COM, 0)){
            // is this token expected?
            if(!lst_is_cm){
                lst_is_cm = T;
                continue;
            } else cmperr(UNEXPCT, frst, nil);

        // it is something unexpected
        } else cmperr(UNEXPCT, frst, nil);
    }
    // spare comma
    if(lst_is_cm) cmperr(UNEXPCT, frst, nil);
    
    lst_is_cm = T;
    // togle between indexer and semicolon
    // until you find the end of the path
    for(;!eq_sym(last, SYM_CLN, 0); last = last->next){
        // indexer
        if(last->type == INDEXER
        or last->type == LITERAL){
            // is this token expected?
            if(lst_is_cm){
                lst_is_cm = F;
                rcnt++;

                pntr->next = malloc(sizeof(node));
                pntr = pntr->next;

                // avoid false positives by jumping over expressions
                if(last->next->type == OPERATOR){
                    astt * expr = exprss_r(last->next, F);
                    last = expr->endo;
                    
                    pntr->next = expr->pstt;
                    pntr = expr->pend;

                    free(expr);
                // just a single indexer or literal
                } else {
                    pntr->vall = last->type;
                    pntr->itsf = last->vall;
                }
                continue;

            } else cmperr(UNEXPCT, last, nil);

        // comma
        } else if(eq_sym(last, SYM_COM, 0)){
            // is this token expected?
            if(!lst_is_cm){
                lst_is_cm = T;
                continue;
            } else cmperr(UNEXPCT, last, nil);

        // it is something unexpected
        } else cmperr(UNEXPCT, frst, nil);
    }
    // spare comma
    if(lst_is_cm) cmperr(UNEXPCT, last, nil);
    
    // check if expression is valid
    if(lcnt != rcnt) cmperr(MSMATCH, c, nil);
    else path->pend = pntr;

    return path;
}
// structure definition hotpath's rule
astt * strdef_r(tkn * c){
    astt * path = nil;
    assert(F, "structures not implemented yet");
    return path;
}
// enum definition hotpath's rule
astt * enumdf_r(tkn * c){
    astt * path = nil;
    assert(F, "enums not implemented yet");
    return path;
}
// structure literal hotpath's rule
astt * struct_r(tkn * c){
    astt * path = nil;
    assert(F, "structures not implemented yet");
    return path;
}
// constant definition hotpath's rule
astt * constd_r(tkn * c){
    astt *path = malloc(sizeof(astt));
    node *pntr = malloc(sizeof(node));
    path->pstt = pntr;

    path->pstt->itsf.num = 0xC05;

    // the sentense is fallowing the path `NAMESPACE ( ... );`
    if(eq_sym(c->next, SYM_PAR, 0)){
        tkn * end = matchpair(c->next);

        // it's a function
        if(eq_sym(end->next, SYM_BRA, 0)
        or end->next->type == INDEXER
        or end->next->type == LITERAL){
            // do not be redundant
            free(pntr);
            // validade path
            astt * func = fun_def_r(c->next);

            path->strt  = func->strt;
            path->endo  = func->endo;

            // append chain loop
            path->pstt = func->pstt;
            path->pend = func->pend;

            free(func);
            return path;

        // a funcall
        } else if(eq_sym(end->next, SYM_CLN, 0)){
            return funcall_r(c->last);

        // invalid syntax
        } else cmperr(UNEXPCT, end->next, nil);
    // check for expressions and values
    } else {
        // an array definition
        if(eq_sym(c->next, SYM_BRA, 0)){
            astt * end = arrdef_r(c->next);
            // TODO: implement arrdef_r and its handlers

            free(end);
            return path;
        }
        // expression
        else if(c->next->next->type == OPERATOR){
            // validade path
            astt * expr = exprss_r(c->next->next, F);

            path->strt   = c;
            path->endo   = expr->endo;

            pntr->vall = CONSTD;
            pntr->itsf = c->vall;
            
            // append chain loop
            pntr->next = expr->pstt;
            pntr = expr->pend;
            path->pend = pntr;

            free(expr);
            return path;
        }

        // TODO: handle function call

        // value
        else if(c->next->type == INDEXER or c->next->type == LITERAL){
            path->strt = c;
            path->endo = c->next;

            // namespace
            pntr->vall = CONSTD;
            pntr->itsf = c->vall;

            // value
            pntr->next = malloc(sizeof(node));
            pntr = pntr->next;
            pntr->vall = c->next->type;
            pntr->itsf = c->vall;

            // finish with metadata
            path->pend = pntr;
            return path;
        } else {
            if(c->next->type == KEYWORD) cmperr(KWRDVAL, c->next, nil);
            else cmperr(UNEXPCT, c->next, nil);
        }
    }
}
// arithmetic and boolean expressions hotpath's rule
astt * exprss_r(tkn * c, bool prnd){
    // redirect if it's an assignment
    if(eq_opr_range(c, asgn))
        return assign_r(c, prnd);
    else if(c->type != OPERATOR)
        cmperr(EXPCTEX, c->last, nil);

    astt * path = malloc(sizeof(astt));
    node * pntr = malloc(sizeof(node));

    path->pstt = pntr;
    pntr->vall = EXPRSS;

    // it's a binary operator
    if(c->last->type == INDEXER
    or c->last->type == LITERAL){

        // append lefthand
        pntr->vall = c->last->type;
        pntr->itsf = c->last->vall;

        pntr->next = malloc(sizeof(node));
        pntr = pntr->next;

        // append operator
        pntr->vall = OPERATOR;
        pntr->itsf = c->vall;

        // both sides are valid members
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            // this is not the entire expression
            if(c->next->next->type == OPERATOR){
                astt * exp = exprss_r(c->next->next, prnd);
                path->endo = exp->endo;

                pntr->next = exp->pstt;
                pntr = exp->pend;

                free(exp);

            // simple expresison
            } else {
                pntr->next = malloc(sizeof(node));
                pntr = pntr->next;
                pntr->vall = c->next->type;
                pntr->itsf = c->next->vall;
                path->endo = c->next;
            }
        // so is righthand invalid?
        } else {
            // the current path fallows the `foo <opr> (bar[ ...])` syntax
            if(eq_sym(c->next, SYM_PAR, 0)){
                // * c->next->next => <opr> >> ( >> <lhd>
                tkn * rhnd = c->next->next;

                pntr->next = malloc(sizeof(node));
                pntr = pntr->next;

                pntr->vall = LSYMBOL;
                pntr->itsf = c->next->vall;

                // righthand is a ``(<opr> foo)`` path
                if(rhnd->type == OPERATOR){
                    astt * exp = exprss_r(rhnd, T);
                    path->endo = exp->endo;

                    pntr->next = exp->pstt;
                    pntr = exp->pend;

                    free(exp);
                // it's really an expression
                } else if(rhnd->next->type == OPERATOR){
                    astt * exp = exprss_r(rhnd->next, T);
                    path->endo = exp->endo;

                    pntr->next = exp->pstt;
                    pntr = exp->pend;

                    free(exp);
                // just a value surrounded by parentheses
                } else {
                    pntr->next = malloc(sizeof(node));
                    pntr->next->vall = rhnd->type;
                    pntr->next->itsf = rhnd->vall;

                    path->endo = rhnd;
                }
            } else {
                // it's an assignment operator
                if(eq_opr_range(c, asgn))
                    cmperr("invalid value for assigment", c->next, nil);
                // something else
                else if(eq_opr_range(c, eqlt))
                    cmperr("righthand cannot be evaluated", c->next, nil);
            }
        }
    // unary
    } else if(eq_opr_range(c, unry)){
        // append the operator
        tkn * after = c->next->next;
        pntr->vall = OPERATOR;
        pntr->itsf = c->vall;
        // raw value
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            if(after->type == OPERATOR){
                // evaluate the righthand
                astt * exp = exprss_r(after, prnd);
                path->endo = exp->endo;

                pntr->next = exp->pstt;
                pntr = exp->pend;

                free(exp);
            } else {
                pntr->next = malloc(sizeof(node));
                pntr = pntr->next;
                pntr->vall = c->next->type;
                pntr->itsf = c->next->vall;

                path->endo = c->next;
            }
        // enclosured by parentheses
        } else if(eq_sym(c->next, SYM_PAR, 0)){
            astt * exp = exprss_r(c->next->next->next, T);
            path->endo = exp->endo;

            pntr->next = exp->pstt;
            pntr = exp->pend;

            free(exp);
        }
    } else cmperr(
        "lefthand of the expression"
        " is missing or invalid", c->next, nil);

    // last syntax check
    if(prnd){
        if(eq_sym(path->endo->next, SYM_PAR, 1)){
            pntr->next = malloc(sizeof(node));
            pntr = pntr->next;
            pntr->vall = LSYMBOL;
            pntr->itsf = path->endo->next->vall;

            path->endo = path->endo->next;
        }
        else
            cmperr(EXPCTDP, path->endo->next, nil);
    }
    // finish with metadata
    path->pend = pntr;
    return path;
}
// array definition hotpath rule
astt * arrdef_r(tkn * c){
    astt * path = nil;
    assert(F, "arrays not implemented yet");
    return path;
}
// statement declaration hotpath rule
astt * sttmnt_r(tkn * c){
    astt * path = nil;
    assert(F, "statements not implemented yet");
    return path;
}
// function definition hotpath rule
astt * fun_def_r(tkn * c){
    astt *path = malloc(sizeof(astt));
    node *pntr = malloc(sizeof(node));
    path->pstt = pntr;

    // end of arguments
    tkn * eoa = matchpair(c);
    // function indexes
    path->strt = c;
    
    // the body is a single line
    if(eoa->next->type == INDEXER
    or eoa->next->type == LITERAL){
        // TODO: handle single lined functions
        assert(F, "single-lined functions not implemented yet");

    // bracket scope
    } else if(eq_sym(eoa->next, SYM_BRA, 0))
        path->endo = matchpair(eoa->next);

    // function call
    else if(eq_sym(eoa->next, SYM_CLN, 0)){
        free(pntr);
        free(path);
        return funcall_r(c->last);

    } else cmperr(UNEXPCT, c, nil);

    // start code path
    pntr->vall = FUNDEF;
    pntr->itsf = c->last->vall;

    pntr->next = malloc(sizeof(node));
    pntr = pntr->next;
    pntr->vall = LSYMBOL;
    pntr->itsf = c->vall;

    // args is not empty
    if(eoa != c->next){
        bool which = 0;
        for(tkn * t = c->next; t != eoa; t = t->next){
            switch (t->type){
                case INDEXER:
                    if(!which){
                        which = T;
                        // default function arguments or computation on assignment
                        if(t->next->type == OPERATOR){
                            astt * exp = exprss_r(t->next, F);

                            pntr->next = exp->pstt;
                            pntr = exp->pend;

                            t = exp->endo;
                            // it's the end of the args
                            if(t == eoa){
                                t = eoa->last;
                                // go back once we can't break the for loop
                                break;
                            }
                        // just the parameter
                        } else {
                            pntr->next = malloc(sizeof(node));
                            pntr = pntr->next;
                            pntr->vall = INDEXER;
                            pntr->itsf = t->vall;
                        }
                    }
                    else cmperr(UNEXPCT, t, nil);
                    break;

                case LSYMBOL:
                    if(which) which = F;
                    else cmperr(UNEXPCT, t, nil); 

                    if(!eq_sym(t, SYM_COM, 0))
                        cmperr(UNEXPCT, t, nil);
                    break;
                default:
                    cmperr(UNEXPCT, t, nil);
            }
        }
    }
    pntr->next = malloc(sizeof(node));
    pntr = pntr->next;
    pntr->vall = LSYMBOL;
    pntr->itsf = eoa->vall;

    // validate the body of the function
    astt *body = parse(eoa->next->next, SCOPE);
    pntr->next = body->pstt;
    path->pend = body->pend;

    free(body);
    return path;
}
// function call hotpath rule
astt * funcall_r(tkn * c){
    astt * path = nil;
    assert(F, "funcall not implemented yet");
    return path;
}
// goto jump label hotpath rule
astt * labeldf_r(tkn * c){
    astt * path = nil;
    return path;
}
// goto jump hotpath rule
astt * jmp_stt_r(tkn * c){
    astt * path = nil;
    return path;
}

// matches the closing or opening token index of a given symbol
tkn * matchpair(tkn * c){
    u64 cnt = 0;
    // it's a openning symbol
    if(c->apdx == 0){
        for(tkn * t = c; t->next != EOTT; t = t->next){
            if(t->type == LSYMBOL and eq_sym(t, c->vall.num, t->apdx)){
                // if different, it's the pair
                if(t->apdx != c->apdx) cnt--;
                else cnt++;

                // if scope level matches, return
                if(cnt == 0) return t;
            }
        }
    // it's a closing one
    } else {
        for(tkn * t; t->last != EOTT; t = t->last){
            // it's a symbol
            if(t->type == LSYMBOL){
                // it's the pair that we're looking for
                if(t->vall.num == c->vall.num){
                    // if different, it's the pair
                    if(t->apdx != c->apdx) cnt--;
                    else cnt++;

                    // if scope level matches, return
                    if(cnt == 0) return t;
                }
            }
        }
    }
    assert(F, "could not find matching pair");
}

// checks if the current path is terminated with a scln
void hasscolon(astt * out){
    assert(out != nil, nil);
    // is an end of line?
    if(!eq_sym(out->endo->next, SYM_CLN, 0))
        cmperr(NOTERMN, out->endo, nil);
}

// returns the token value as a string
char * get_tokval(tkn * tok){
    switch(tok->type){
        case KEYWORD:
            return KEYWORDS[tok->vall.num];
        case INDEXER:
            return tok->vall.str;
        case LITERAL:
            if(tok->apdx == FREEABLE) return tok->vall.str;
            else return sarr.arr[tok->vall.num];
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

astt * parse(tkn * tkns, cmod mode){
    // if any rule breaks, retreat parsing until here
    tkn * ctok = tkns, * eos;
    astt *ctxt = malloc(sizeof(astt));

    node * pntr = malloc(sizeof(node));
    ctxt->pstt = pntr;

    // config code chuck
    if(mode == SCOPE){
        eos = matchpair(tkns->last);
        pntr->vall = BODYDF;
    } else {
        eos = EOTT;
        pntr->vall = CODEIS;
    }

    bool mtch = T;
    tkn * tok, * old = nil;

    void * lbls[] = {
        &&case_KEYWORD ,
        &&case_INDEXER ,
        &&case_LITERAL ,
        &&case_LSYMBOL ,
        &&case_OPERATOR,
        &&case_EOTT
    };
    #define nxt(t)\
        t = t->next;\
        goto *lbls[t->type];

    tok = tkns;
    goto *lbls[tok->type];

    // statements and definitions
    case_KEYWORD:
        // local definition
        if(eq_kwd(tok, ldef)){
            astt * def = define_r(tok);

            // append to chain and move to the next link
            pntr->next = def->pstt;
            pntr = def->pend;

            hasscolon(def);
            tok = def->endo;
            free(def);

        // statements
        } else if(eq_kwd(tok, sttt)){
            astt * stt = sttmnt_r(tok);

            pntr->next = stt->pstt;
            pntr = stt->pend;

            // is an end of line?
            hasscolon(stt);
            tok = stt->endo;
            free(stt);

        // otherwise is not
        } else cmperr(ALONEXP, tok->last, nil);
        nxt(tok);


    // if it's an indexer, it's a constant definition
    // or an assignment, so check for both cases.
    case_INDEXER:
        // skip this so then the next case can take care
        if(tok->next->type == OPERATOR){
            nxt(tok);
        } else {
            astt * def = constd_r(tok);

            pntr->next = def->pstt;
            pntr = def->pend;

            // is an end of line?
            hasscolon(def);
            tok = def->endo;
            free(def);
        }
        nxt(tok);

    case_LITERAL:
        cmperr(UNEXPCT, tok, nil);

    case_LSYMBOL:
        if(eq_sym(tok, SYM_BRA, 0)){
            astt * scp = parse(tok->next, SCOPE);

            pntr->next = scp->pstt;
            pntr = scp->pend;

            // is an end of line?
            hasscolon(scp);
            tok = scp->endo;
            free(scp);

        // it's the end of the scope
        } else if(eq_sym(tok, SYM_BRA, 1)){
            // finish if it's the end and we're within a scope
            if(mode == SCOPE) goto *lbls[UNKNOWN];
            else cmperr(UNEXPCT, tok, nil);

        // just a terminator, next token!
        } else if(eq_sym(tok, SYM_CLN, 0)){
            nxt(tok);
        } else
            cmperr(UNEXPCT, tok, nil);
        nxt(tok);

    // expressions
    case_OPERATOR:
        // an assigment is allowed out of an statement
        if(eq_opr_range(tok, asgn)){
            astt * exp = assign_r(tok, F);

            pntr->next = exp->pstt;
            pntr = exp->pend;

            // is an end of line?
            hasscolon(exp);
            tok = exp->endo;
            free(exp);

        // other operations are not
        } else cmperr(ALONEXP, tok, nil);
        nxt(tok);

    case_EOTT:
        ctxt->endo = eos;
        if(mode != SCOPE){
            pntr->next = ctxt->pstt;
            ctxt->pstt->last = pntr;
        }
        ctxt->pend = pntr;

    return ctxt;
}