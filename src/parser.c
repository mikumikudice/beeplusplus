#ifndef BIC
    #include "bic.h"
#endif

stra namespaces;

// code path rules
rulr define_r(tkn * c, bool jchk){
    rulr path = {};
    // left hand of the expresison or the
    // right hand of the statement is valid
    if(c->next->type == INDEXER){
        // the next token is an operator, so it's an assigment
        if(c->next->next->type == OPERATOR){
            rulr exp = exprss_r(c->next->next, T, F);
            if(!jchk){
                path.out = malloc(sizeof(node));

                path.out->path = malloc(sizeof(u16) * 3);
                path.out->path[0] = KEYWORD;
                path.out->path[1] = EXPRSS;
                path.out->path[2] = EOLINE;

                path.out->strt = c;
                path.out->end_of = exp.end;
                path.out->path_t = DEFINE;
            }
            path.end = exp.end;
            return path;
        }
        else if(c->next->next->type == LSYMBOL){
            // TODO: handle foo, bar = x, y syntax

            // no terminator
            if(c->next->next->vall.num != SYM_CLN)
                cmperr(NOTERMN, c->next->next, nil);

            if(!jchk){
                path.out = malloc(sizeof(node));

                path.out->path = malloc(sizeof(u16) * 3);
                path.out->path[0] = KEYWORD;
                path.out->path[1] = INDEXER;
                path.out->path[2] = EOLINE;

                path.out->strt = c;
                path.out->end_of = c->next;
                path.out->path_t = DEFINE;
            }
            path.end = c->next;
            return path;
        }
    // rule didn't failed, but the syntax did
    } else cmperr(
        "invalid definition namespace", 
        c->next, nil
    );
    return path;
}

rulr assign_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr sttdef_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr enumdf_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr struct_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}

rulr constd_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}

rulr exprss_r(tkn * c, bool jchk, bool prnd){
    rulr path = {};
    // it's a binary operator
    if(c->last->type == INDEXER
    or c->last->type == LITERAL){
        // both sides are valid members
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            // this is not the entire expression
            if(c->next->next->type == OPERATOR){
                rulr exp = exprss_r(c->next->next, T, F);
                // when not _just checking_
                if(!jchk){
                    path.out = malloc(sizeof(node));

                    // code path in numeric types
                    path.out->path = malloc(sizeof(u16) * 3);
                    path.out->path[0] = c->last->type; // foo
                    path.out->path[1] = c->vall.num;   // [opr]
                    path.out->path[2] = EXPRSS;        // bar

                    path.out->strt   = c->last;
                    path.out->end_of = exp.end;
                    path.out->path_t = EXPRSS;
                }
                // we are within a parenthesis block
                if(prnd){
                    if( exp.end->next->type == LSYMBOL
                    and exp.end->next->vall.num == SYM_PAR
                    and exp.end->next->apdx == 1)
                        // move the end pointer to the parenthesis
                        exp.end = exp.end->next;
                }
                path.end = exp.end;
                return path;
            // simple expresison
            } else {
                // when not _just checking_
                if(!jchk){
                    path.out = malloc(sizeof(node));

                    // code path in numeric types
                    path.out->path = malloc(sizeof(u16) * 3);
                    path.out->path[0] = c->last->type; // foo
                    path.out->path[1] = c->vall.num;   // [opr]
                    path.out->path[2] = c->next->type; // bar

                    path.out->strt   = c->last;
                    path.out->end_of = c->next;
                    path.out->path_t = EXPRSS;
                }
                // we are within a parenthesis block
                path.end = c->next;
                if(prnd){
                    if( c->next->next->type == LSYMBOL
                    and c->next->next->vall.num == SYM_PAR
                    and c->next->next->apdx == 1)
                        // move the end pointer to the parenthesis
                        path.end = c->next->next;
                }
                return path;
            }
        // right hand is invalid
        } else {
            // the current path fallows the `foo [opr] (bar[ ...])` syntax
            if( c->next->type == LSYMBOL
            and c->next->vall.num == SYM_PAR
            and c->next->apdx == 0){
                // just check if right hand is valid
                // * c->next->next->next => [opr]->[%(]->[lhd]
                rulr lhnd = exprss_r(c->next->next->next, T, T);
                
                // check if the expresison parenthesis is closed properly
                if( lhnd.end->type == LSYMBOL
                and lhnd.end->vall.num == SYM_PAR
                and lhnd.end->apdx == 1){
                        // move the pointer to the true end. if
                        // the true end is not an semicolon the
                        // caller takes care of it for us.
                        if( lhnd.end->next->type == LSYMBOL
                        and lhnd.end->next->vall.num == SYM_CLN){
                            lhnd.end = lhnd.end->next;
                        }
                }
                // when not _just checking_
                if(!jchk){
                    path.out = malloc(sizeof(node));

                    // code path in numeric types
                    path.out->path = malloc(sizeof(u16) * 3);
                    path.out->path[0] = c->last->type; // foo
                    path.out->path[1] = c->vall.num;   // [opr]
                    path.out->path[2] = EXPRSS;        // bar

                    path.out->strt   = c->last;
                    path.out->end_of = lhnd.end;
                    path.out->path_t = EXPRSS;
                }
                path.end = lhnd.end;
                return path;
            // regardless of what comes after it, it's an unexpected thing
            } else cmperr(UNEXPCT, c, nil);

            u16 opr = c->vall.num;

            // it's an assignment operator
            if(is_ass_opr(opr))
                cmperr("invalid value for assigment", c->next, nil);
            else if(is_eql_opr(opr))
                cmperr("right hand cannot be evaluated", c->next, nil);
        }
    // unary
    } else if(is_unr_opr(c->vall.num)){
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            // +foo = bar is invalid
            if((c->last->type == LSYMBOL and c->last->vall.num == SYM_CLN)
            or (c->last == EOTT)){
                cmperr(
                    "an expression may not "\
                    "start with an operator", c, nil
                );
            }
            // when not _just checking_
            if(!jchk){
                path.out = malloc(sizeof(node));
                // code path in numeric types
                path.out->path = malloc(sizeof(u16) * 3);
                path.out->path[0] = c->last->type; // foo
                path.out->path[1] = c->vall.num;   // [opr]
                path.out->path[2] = c->next->type; // bar

                path.out->strt   = c->last;
                path.out->end_of = c->next;
                path.out->path_t = EXPRSS;
            }
            path.end = c->next;
            return path;
        }
    } else {
        cmperr("missing left hand of expression", c, nil);
    }
    return path;
}

rulr arrdef_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr sttmnt_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr fundef_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr funcall_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr labeldf_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}
rulr jmp_stt_r(tkn * c, bool jchk){
    rulr path= {};
    return path;
}

tkn * matchpair(tkn * c){
    u64 cnt = 1;
    // it's a openning symbol
    if(c->apdx == 0){
        for(tkn * t; t->next != EOTT; t = t->next){
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
    return nil;
}

/* this function is a very expensive one, *\
*  so only call it at the end of the rule  *
*  ======================================  *
*  consumes the nodes between i and f and  *
\* makes the ast shorter by squeezing it. */
astt * flush_ast(astt * ctxt, u64 i, u64 f){
    u64 len = ctxt->clen;
    for(u64 n = i + 1; n <= f; n++){
        free(ctxt->ctxt[n]->strt);
        ctxt->ctxt[n]->strt = nil;
        len--;
    }
    astt * out = malloc(sizeof(astt));
    out->ctxt = malloc(len * sizeof(node));

    for(u64 n = 0; n < ctxt->clen; n++){
        if(ctxt->ctxt[n]->strt)
            out->ctxt[out->clen++] = ctxt->ctxt[n];
    }
    free(ctxt);
    return out;
}

// TODO: check if either the rule failed or it's a syntax error
void todo();
void todo(){puts("reached TODO");}

char * get_tokval(tkn * tok){
    switch (tok->type){
        case KEYWORD:
            return KEYWORDS[tok->vall.num];
        case INDEXER:
            return tok->vall.str;
        case LITERAL:
            if(tok->apdx == FREEABLE) return tok->vall.str;
            else return sarr.arr[tok->vall.num];
        case LSYMBOL:
            if(tok->apdx == 0) return SYMBOLS[tok->vall.num].s;
            else return SYMBOLS[tok->vall.num].e;
        case OPERATOR:
            return OPERATORS[tok->vall.num];
        default:
            return "NONE";
    }
}

astt parse(tkn * tkns, cmod mode){
    // init array
    namespaces.arr = malloc(sizeof(char *) * EOTT->apdx / 2);

    // if any rule breaks, retreat parsing until here
    tkn * safe = tkns, * ctok = tkns;
    astt  ctxt;
    ctxt.ctxt = malloc(EOTT->apdx * sizeof(node));
    ctxt.clen = 0;

    bool mtch = T;

    tkn *tok, * old = nil;
    // parse by line, simplifying code into nodes
    for(tok = tkns; tok != EOTT; tok = tok->next){
        switch(tok->type){
            // if it's an indexer, it's a constant definition
            // or an assignment, so check for both cases.
            case INDEXER:
                // skip this so then the next case can take care
                if(tok->next->type == OPERATOR) continue;
                else {
                    rulr def = constd_r(tok, F);


                }
                break;

            // expressions
            case OPERATOR:
                // an assigment is allowed out of an statement
                if(is_ass_opr(tok->vall.num)){
                    rulr exp = exprss_r(tok, F, F);
                    
                    ctxt.ctxt[ctxt.clen] = exp.out;
                    ctxt.clen++;

                    // is an end of line?
                    if(!(exp.end->type == LSYMBOL
                    and  exp.end->vall.num == SYM_CLN))
                        cmperr(NOTERMN, exp.end, nil);

                    tok = exp.end;
                // otherwise is not
                } else cmperr(ALONEXP, tok->last, nil);
                break;

            // statements
            case KEYWORD:
                if(tok->vall.num == KW_AUTO){
                    rulr def = define_r(tok, F);

                    ctxt.ctxt[ctxt.clen] = def.out;
                    ctxt.clen++;
                    tok = def.end;
                }
            default: break;
        }
    }
    return ctxt;
}