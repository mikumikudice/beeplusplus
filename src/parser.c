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

// appends a token at the end of the path as a node
#define tok_to_node(n, t){\
    n->next = malloc(sizeof(node));\
    n->next->last = n;\
    n = n->next;\
    n->type = t->type;\
    n->vall = t->vall;\
    n->next = nil;\
    n->last = nil;\
    n->stt  = nil;\
    n->end  = nil;\
    n->is_parent = F;\
}

// code path rules

node * define_r(tkn * c){
    node *path = malloc(sizeof(node));
    node *pntr = malloc(sizeof(node));

    pntr->next = (path->next = nil);
    pntr->last = (path->last = nil);
    pntr->stt  = (path->stt  = nil);
    pntr->end  = (path->end  = nil);

    // basic metadata
    pntr->is_parent = T;
    path->type = DEFINE;
    path->ftok = c;

    // start branches
    path->stt  = pntr;
    pntr->type = KEYWORD;
    pntr->vall = c->vall;
    pntr->is_parent = F;

    // definition has attribution
    if(c->next->next->type == OPERATOR){
        if(!eq_opr(c->next->next, 0, 0))
            cmperr(DEFWOEQ, c->next->next, nil);

        // a definition is an keyword and an assignment
        // so evaluate the last part and append the kwd
        pntr->next = assign_r(c->next->next, F);
        path->ltok = pntr->next->ltok;
        path->end  = pntr->next->end;

    // only namespace
    } else if(c->next->type == INDEXER){
        path->ltok = c->next;
        path->end  = pntr;
        tok_to_node(pntr, c->next);
    }

    path->end->next = path->stt;
    path->stt->last = path->end;
    return path;
}

// assignment hotpath's rule
node * assign_r(tkn * c, bool prnd){
    // validate operator
    if(!eq_opr_range(c, asgn))
        cmperr(NOTASGN, c, nil);

    node *path = malloc(sizeof(node));
    node *pntr = malloc(sizeof(node));

    path->is_parent = T;
    path->type = ASSIGN;
    path->stt  = pntr;

    tkn *strt = c->last, *prox = c->next->next;

    // Type A: <type> foo = bar [, spm = egg]*

    // validate lefthand

    // the last token before the assignment is a square
    // bracket, a comma, a semicolon or a keyword.
    if(eq_sym(strt->last, SYM_CLN, 0)
    or eq_sym(strt->last, SYM_COM, 0)
    or eq_sym(strt->last, SYM_SQR, 0)
    or eq_kwd(strt->last, ldef)){
        // append the operation's lefthand
        strt = (path->ftok = c->last);
        pntr->type = strt->type;
        pntr->vall = strt->vall;
        pntr->is_parent = F;

        // now the operator itself
        tok_to_node(pntr, c);

    // invalid position for an assignment
    } else cmperr(
        "this assignment is"
        " at an invalid position", strt, nil);

    // validate the righthand
    if(c->next->type == INDEXER or c->next->type == LITERAL){
        // check for expressions
        if(prox->type == OPERATOR){
            // invalid operators
            if(prox->vall.num == 29 or prox->vall.num == 31)
                cmperr(UNEXPCT, prox, nil);

            pntr->next = exprss_r(prox, F);
            pntr = pntr->next;
            prox = pntr->ltok->next;

        // another sentence
        } else
            // push righthand
            tok_to_node(pntr, c->next);

            // foo = bar + buzz, egg = bar + spam, ...
            // ................^
            //                 you are here

            if(eq_sym(prox, SYM_COM, 0)
            or eq_sym(prox, SYM_CLN, 0)) goto finish;
            else cmperr(UNEXPCT, prox, nil);

    // the righthand is an expression
    } else if(eq_sym(c->next, SYM_PAR, 0)){
        // an encapsulated signed value
        if(prox->type == OPERATOR){
            pntr->next = exprss_r(prox, T);
            pntr = pntr->next;

        // it's really an expression
        } else if(prox->next->type == OPERATOR){
            pntr->next = exprss_r(prox->next, T);
            pntr = pntr->next;

        // just a value surrounded by parentheses
        } else if(prox->type == LITERAL or prox->type == INDEXER){
            tok_to_node(pntr, prox);
            // after the tok_to_node, the
            // pntr's last token field is
            // null (other cases aren't).
            pntr->ltok = prox;
        }
        prox = pntr->ltok;
    }

    finish:
    // check for multiple definitions
    while(eq_sym(prox, SYM_COM, 0)){
        // evaluate next assignation
        pntr->next = assign_r(prox, F);

        // append it
        pntr = pntr->next;
        prox = pntr->ltok->next;
    }

    if(prnd){
        if(eq_sym(prox, SYM_PAR, 1)){
            tok_to_node(pntr, prox);
            prox = prox->next;
        } else cmperr(EXPCTDP, prox, nil);
    }

    // TODO: handle multiple assignment

    path->ltok = prox->last;
    path->end  = pntr;

    path->end->next = path->stt;
    path->stt->last = path->end;

    assert(path->stt == path->end->next, nil);
    return path;
}
// structure definition hotpath's rule
node * strdef_r(tkn * c){
    node * path = nil;
    assert(F, "structures not implemented yet");
    return path;
}
// enum definition hotpath's rule
node * enumdf_r(tkn * c){
    node * path = nil;
    assert(F, "enums not implemented yet");
    return path;
}
// structure literal hotpath's rule
node * struct_r(tkn * c){
    node * path = nil;
    assert(F, "structures not implemented yet");
    return path;
}
// constant definition hotpath's rule
node * constd_r(tkn * c){
    node *path = malloc(sizeof(node));
    node *pntr = malloc(sizeof(node));

    pntr->next = (path->next = nil);
    pntr->last = (path->last = nil);
    pntr->stt  = (path->stt  = nil);
    pntr->end  = (path->end  = nil);

    path->is_parent = T;
    path->type = CONSTD;
    path->ftok = c;
    path->stt  = pntr;

    pntr->type = INDEXER;
    pntr->vall = c->vall;
    pntr->is_parent = F;

    // the sentense is fallowing the path `NAMESPACE ( ... );`
    if(eq_sym(c->next, SYM_PAR, 0)){
        tkn * end = matchpair(c->next);

        // it's a function
        if(eq_sym(end->next, SYM_BRA, 0)
        or end->next->type == INDEXER
        or end->next->type == LITERAL){
            // do not be redundant
            free(path);
            free(pntr);
            return fun_def_r(c->next);

        // a funcall
        } else if(eq_sym(end->next, SYM_CLN, 0)){
            free(path);
            free(pntr);
            return funcall_r(c);

        // invalid syntax
        } else cmperr(UNEXPCT, end->next, nil);

    // an array definition
    } else if(eq_sym(c->next, SYM_BRA, 0)){
        free(path);
        free(pntr);
        return arrdef_r(c->next);

    // expression
    } else if(c->next->next->type == OPERATOR){
        // validade path
        pntr->next = exprss_r(c->next->next, F);
        pntr = pntr->next;

        // finish with metadata
        path->ltok = pntr->ltok;
        pntr->end  = pntr;

        path->end->next = path->stt;
        path->stt->last = path->end;
        return path;

    // value
    } else if(c->next->type == INDEXER or c->next->type == LITERAL){
        tok_to_node(pntr, c->next);
        
        // finish with metadata
        path->ltok = c->next;
        pntr->end  = pntr;

        path->end->next = path->stt;
        path->stt->last = path->end;
        return path;

    // invalid token
    } else {
        if(c->next->type == KEYWORD) cmperr(KWRDVAL, c->next, nil);
        else cmperr(UNEXPCT, c->next, nil);
    }
}
// arithmetic and boolean expressions hotpath's rule
node * exprss_r(tkn * c, bool prnd){
    // redirect if it's an assignment
    if(eq_opr_range(c, asgn))
        return assign_r(c, prnd);
    else if(c->type != OPERATOR)
        cmperr(EXPCTEX, c->last, nil);

    node * path = malloc(sizeof(node));
    node * pntr = malloc(sizeof(node));

    pntr->next = (path->next = nil);
    pntr->last = (path->last = nil);
    pntr->stt  = (path->stt  = nil);
    pntr->end  = (path->end  = nil);

    path->is_parent = T;
    path->stt  = pntr;
    path->type = EXPRSS;

    // it's a binary operator
    if(c->last->type == INDEXER
    or c->last->type == LITERAL){
        // append lefthand
        pntr->type = c->last->type;
        pntr->vall = c->last->vall;
        pntr->is_parent = F;

        // append operator
        tok_to_node(pntr, c);

        // both sides are valid members
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            // this is not the entire expression
            if(c->next->next->type == OPERATOR){
                pntr->next = exprss_r(c->next->next, prnd);
                pntr = pntr->next;
                path->ltok = pntr->ltok;

            // simple expression
            } else {
                tok_to_node(pntr, c->next);
                path->ltok = c->next;
            }
        // so is righthand invalid?
        } else {
            // the current path fallows the `foo <opr> (bar[ ...])` syntax
            if(eq_sym(c->next, SYM_PAR, 0)){
                // * c->next->next => <opr> >> ( >> <lhd>
                tkn * rhnd = c->next->next;
                tok_to_node(pntr, rhnd->last);

                // righthand is a ``(<opr> foo)`` path
                if(rhnd->type == OPERATOR){
                    pntr->next = exprss_r(rhnd, T);
                    pntr = pntr->next;
                    path->ltok = pntr->ltok;

                // it's really an expression
                } else if(rhnd->next->type == OPERATOR){
                    pntr->next = exprss_r(rhnd->next, T);
                    pntr = pntr->next;
                    path->ltok = pntr->ltok;

                // just a value surrounded by parentheses
                } else if(rhnd->type == INDEXER or rhnd->type == LITERAL) {
                    tok_to_node(pntr, rhnd);
                    path->ltok = rhnd;
                }
            // errors
            } else {
                // it's an assignment operator
                if(eq_opr_range(c, asgn))
                    cmperr("invalid value for assignment", c->next, nil);
                // something else
                else if(eq_opr_range(c, eqlt))
                    cmperr("righthand cannot be evaluated", c->next, nil);
            }
        }
    // unary
    } else if(eq_opr_range(c, unry)){
        // append the operator
        tkn * after = c->next->next;
        
        pntr->type = OPERATOR;
        pntr->vall = c->vall;

        // single value
        if(c->next->type == INDEXER
        or c->next->type == LITERAL){
            if(after->type == OPERATOR){
                // evaluate the righthand
                pntr->next = exprss_r(after, prnd);
                pntr = pntr->next;
                path->ltok = pntr->ltok;

            } else {
                tok_to_node(pntr, c->next);
                path->ltok = c->next;
            }

        // cloistered by parentheses
        } else if(eq_sym(c->next, SYM_PAR, 0)){
            pntr->next = exprss_r(c->next->next->next, T);
            pntr = pntr->next;
            path->ltok = pntr->ltok;
        }

    } else cmperr(
        "lefthand of the expression"
        " is missing or invalid", c->next, nil);

    // last syntax check
    if(prnd){
        if(eq_sym(path->ltok->next, SYM_PAR, 1)){
            tok_to_node(pntr, path->ltok->next);
            path->ltok = path->ltok->next;
        }
        else cmperr(EXPCTDP, path->ltok->next, nil);
    }

    // finish with metadata
    path->end = pntr;

    path->end->next = path->stt;
    path->stt->last = path->end;
    return path;
}
// array definition hotpath rule
node * arrdef_r(tkn * c){
    node * path = nil;
    assert(F, "arrays not implemented yet");
    return path;
}
// statement declaration hotpath rule
node * sttmnt_r(tkn * c){
    node * path = nil;
    assert(F, "statements not implemented yet");
    return path;
}
// function definition hotpath rule
node * fun_def_r(tkn * c){
    node *path = malloc(sizeof(node));
    node *pntr = malloc(sizeof(node));

    pntr->next = (path->next = nil);
    pntr->last = (path->last = nil);
    pntr->stt  = (path->stt  = nil);
    pntr->end  = (path->end  = nil);

    path->is_parent = T;
    path->stt  = pntr;
    path->type = FUNDEF;
    path->ftok = c->last;

    // end of arguments
    tkn * eoa = matchpair(c);
    
    // the body is a single line
    if(eoa->next->type == INDEXER
    or eoa->next->type == LITERAL){
        // TODO: handle single lined functions
        assert(F, "single-lined functions not implemented yet");

    // function call
    } else if(eq_sym(eoa->next, SYM_CLN, 0)){
        free(pntr);
        free(path);
        return funcall_r(c->last);

    // bracket scope
    } else if(eq_sym(eoa->next, SYM_BRA, 0)){
        path->ltok = matchpair(eoa->next);

    } else cmperr(UNEXPCT, c, nil);

    // start code path
    pntr->type = INDEXER;
    pntr->vall = c->last->vall;
    pntr->is_parent = F;

    // opening parentheses
    tok_to_node(pntr, c);

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
                            if(!eq_opr_range(t->next, asgn))
                                cmperr(NOTASGN, t->next, nil);

                            node * exp = exprss_r(t->next, F);
                            pntr->next = exp;
                            pntr = exp->next;
                            t = exp->ltok;

                        // just the parameter
                        } else tok_to_node(pntr, t);
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

    // closing parentheses
    tok_to_node(pntr, eoa);

    // validate the body of the function
    node *body = parse(eoa->next->next, SCOPE);
    pntr->next = body->stt;
    path->ltok = body->ltok;

    path->end = body->end;
    free(body);

    path->end->next = path->stt;
    path->stt->last = path->end;
    return path;
}
// function call hotpath rule
node * funcall_r(tkn * c){
    node *path = malloc(sizeof(node));
    node *pntr = malloc(sizeof(node));

    pntr->next = (path->next = nil);
    pntr->last = (path->last = nil);
    pntr->stt  = (path->stt  = nil);
    pntr->end  = (path->end  = nil);

    path->is_parent = T;
    path->stt = pntr;
    path->ftok = c;
    path->type = FNCALL;

    // end of arguments
    tkn * eoa = matchpair(c->next);

    // append function name
    pntr->type = INDEXER;
    pntr->vall = c->vall;
    pntr->is_parent = F;

    // opening parentheses
    tok_to_node(pntr, c->next);
    
    // the body is a single line
    if(eoa != c->next->next){
        // the function call args handling's just
        // like the function def params handling,
        // but you seek for exps or literals, not
        // assignments
        bool which = 0;
        for(tkn * t = c->next->next; t != eoa; t = t->next){
            switch (t->type){
                case INDEXER:
                    if(!which){
                        which = T;
                        // default function arguments or computation on assignment
                        if(t->next->type == OPERATOR){
                            node * exp = exprss_r(t->next, F);

                            pntr->next = exp;
                            pntr = exp->next;

                            t = exp->ltok;

                        // just the parameter
                        } else tok_to_node(pntr, t);
                    } else cmperr(UNEXPCT, t, nil);
                    break;

                case LITERAL:
                    if(!which){
                        which = T;
                        tok_to_node(pntr, t);
                    
                    } else cmperr(UNEXPCT, t, nil); 
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

    // closing parentheses
    tok_to_node(pntr, eoa);

    path->ltok = eoa;
    path->end = pntr;

    path->end->next = path->stt;
    path->stt->last = path->end;
    return path;
}
// goto jump label hotpath rule
node * labeldf_r(tkn * c){
    node * path = nil;
    return path;
}
// goto jump hotpath rule
node * jmp_stt_r(tkn * c){
    node * path = nil;
    return path;
}

// matches the closing or opening token index of a given symbol
tkn * matchpair(tkn * c){
    u64 cnt = 0;
    assert(c->type == LSYMBOL, "the given token is not paired");

    // it's a opening symbol
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
        for(tkn * t = c; t->last != EOTT; t = t->last){
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

// checks if the current path is terminated with a semicolon
void hasscolon(node * out){
    assert(out != nil && out->is_parent, "invalid node to evaluate");
    // is an end of line?
    if(!eq_sym(out->ltok->next, SYM_CLN, 0))
        cmperr(NOTERMN, out->ltok, nil);
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

node *parse(tkn * tkns, cmod mode){
    // if any rule breaks, retreat parsing until here
    tkn  *ctok = tkns, * eos;
    node *ctxt = malloc(sizeof(node));
    node *pntr = nil;
    ctxt->stt = pntr;

    ctxt->is_parent = T;
    ctxt->type = (mode == SCOPE ? BODYDF : CODEIS);

    // config code chuck
    if(mode == SCOPE){
        eos = matchpair(tkns->last);
        ctxt->type = BODYDF;
    } else {
        eos = EOTT;
        ctxt->type = CODEIS;
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
            // avoid memory leaking
            if(ctxt->stt){
                pntr->next = define_r(tok);
                pntr->next->last = pntr;
                pntr = pntr->next;
            } else {
                ctxt->stt = define_r(tok);
                pntr = ctxt->stt;
            }

            // there is an end of line
            hasscolon(pntr);
            tok = pntr->ltok;

        // statements
        } else if(eq_kwd(tok, sttt)){
            // avoid memory leaking
            if(ctxt->stt){
                pntr->next = sttmnt_r(tok);
                pntr->next->last = pntr;
                pntr = pntr->next;
            } else {
                ctxt->stt = sttmnt_r(tok);
                pntr = ctxt->stt;
            }
            
            // there is an end of line?
            hasscolon(pntr);
            tok = pntr->ltok;

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
            // avoid memory leaking
            if(ctxt->stt){
                pntr->next = constd_r(tok);
                pntr->next->last = pntr;
                pntr = pntr->next;
            } else {
                ctxt->stt = constd_r(tok);
                pntr = ctxt->stt;
            }

            // there is an end of line?
            hasscolon(pntr);
            tok = pntr->ltok;
        }
        nxt(tok);

    case_LITERAL:
        cmperr(UNEXPCT, tok, nil);

    case_LSYMBOL:
        if(eq_sym(tok, SYM_BRA, 0)){
            // avoid memory leaking
            if(ctxt->stt){
                pntr->next = parse(tok, SCOPE);
                pntr->next->last = pntr;
                pntr = pntr->next;
            } else {
                ctxt->stt = parse(tok, SCOPE);
                pntr = ctxt->stt;
            }

            // there is an end of line?
            hasscolon(pntr);
            tok = pntr->ltok;

        // it's the end of the scope
        } else if(eq_sym(tok, SYM_BRA, 1)){
            // finish if it's the end and we're within a scope
            if(mode == SCOPE){
                if(matchpair(tok) == tkns){
                    goto *lbls[UNKNOWN];
                } else {
                    nxt(tok);
                }
            }
            else cmperr(UNEXPCT, tok, nil);

        // just a terminator, next token!
        } else if(!eq_sym(tok, SYM_CLN, 0))
            cmperr(UNEXPCT, tok, nil);

        nxt(tok);

    // expressions
    case_OPERATOR:
        // an assignment is allowed out of an statement
        if(eq_opr_range(tok, asgn)){
            // avoid memory leaking
            if(ctxt->stt){
                pntr->next = assign_r(tok, F);
                pntr->next->last = pntr;
                pntr = pntr->next;
            } else {
                ctxt->stt = assign_r(tok, F);
                pntr = ctxt->stt;
            }

            // there is an end of line?
            hasscolon(pntr);
            tok = pntr->ltok;

        // other operations are not
        } else cmperr(ALONEXP, tok, nil);
        nxt(tok);

    case_EOTT:
        assert(tok == EOTT, nil);

        pntr->next = malloc(sizeof(node));
        pntr->next->last = pntr;
        pntr = pntr->next;

        pntr->type = (mode == SCOPE ? EOSCPE : CDHALT);
        pntr->is_parent = F;

        ctxt->ltok = eos;
        ctxt->end  = pntr;

    return ctxt;
}