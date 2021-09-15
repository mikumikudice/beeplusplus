#ifndef BIC
    #include "bic.h"
#endif

#define KWRD(isv, skp) (rule){KEYWORD , TOKENT, isv, skp}
#define IDXR(isv, skp) (rule){INDEXER , TOKENT, isv, skp}
#define OPER(isv, skp) (rule){OPERATOR, TOKENT, isv, skp}
#define LITR(isv, skp) (rule){LITERAL , TOKENT, isv, skp}
#define SYMB(isv, skp) (rule){LSYMBOL , TOKENT, isv, skp}

#define ASGN(isv, skp) (rule){ASSGN   , FLANGT, isv, skp}
#define CALL(isv, skp) (rule){FCALL   , FLANGT, isv, skp}
#define NDEF(isv, skp) (rule){NSDEF   , FLANGT, isv, skp}
#define EXPR(isv, skp) (rule){EXPRS   , FLANGT, isv, skp}
#define STMT(isv, skp) (rule){STTMT   , FLANGT, isv, skp}
#define BODY(isv, skp) (rule){SBODY   , FLANGT, isv, skp}

#define COLN(isv, skp) (rule){SYM_CLN , TOKENL, isv, skp}

#define EOAR (rule){-1}

// parser rules
// ==========================================
// A hash table of rule objects (fake array).
// These objects that store in its first item
// the count of items in the array, and after
// that the array content itself.
// An union type is used to store the data of
// how many types an rule node can be

// [(INDEXER,*)+ [+-*/]*=[&|~^<>]* (INDEXER | VALUE | EXPRESSION,*)+]
rule assgn[][4] = {
    {IDXR(T, F), LITR(T, F), EXPR(T, F), EOAR}, // arg 1
    {OPER(F, F), EOAR},                         // arg 2
    {IDXR(T, F), LITR(T, F), EXPR(T, F), EOAR}  // arg 3
};

// [INDEXER (INDEXER | VALUE | EXPRESSION | BODY)]
rule cnsta[][5] = {
    {{3, KEYWRD, T, F}, EOAR},                              // arg 1
    {IDXR(T, T) , LITR(T, T), EXPR(T, T), BODY(T, T), EOAR} // arg 2
};

// TODO: implement array and struct assignment rules

// [INDEXER %( (INDEXER | VALUE | EXPRESSION)* %) ]
rule fcall[][4] = {
    {IDXR(F, F) , EOAR},                         // arg 1: function name
    {{SYM_PAR, TOKENL, F, F}, EOAR},             // arg 2: "("
    {IDXR(T, T) , LITR(T, T), EXPR(T, T), EOAR}, // arg 3: args
    {{SYM_PAR, TOKENL, F, F}, EOAR},             // arg 4: ")"
};

// ["AUTO" ASSIGNMENT]
rule nsdef[][2] = {
    {{3, KEYWRD, F, F}, EOAR},  // arg 1
    {ASGN(0, T), EOAR},         // arg 2
};

// [(INDEXER | VALUE | EXPRESSION) OPERATOR (INDEXER | VALUE | EXPRESSION)]
rule exprs[][4] = {
    {{SYM_PAR, TOKENL, F, F}, EOAR},             // arg 1
    {IDXR(T, F) , LITR(T, F), EXPR(T, F), EOAR}, // arg 2
    {{OPERATOR, TOKENT, F, F}, EOAR},            // arg 3
    {IDXR(T, F) , LITR(T, F), EXPR(T, F), EOAR}, // arg 4
    {{SYM_PAR, TOKENL, F, F}, EOAR},             // arg 5
};

// [KEYWORD %(? DEFINITION? EXPRESSION %) BODY]
rule sttmt[][2] = {
    {KWRD(F, F), EOAR},

    {{SYM_PAR, TOKENL, F, T}, EOAR},

    {NDEF(T, F), EOAR},
    {COLN(F, T), EOAR},

    {EXPR(F, F), EOAR},
    {COLN(F, T), EOAR},

    {EXPR(F, T), EOAR},

    {{SYM_PAR, TOKENL, F, T}, EOAR},

    {BODY(F, F), EOAR}
};

// [ { (ASSIGNMENT | CALL | DEFINITION | EXPRESSION | STATEMENT)* } ]
rule sbody[][4] = {
    {{SYM_BRA, TOKENL, F, F}, EOAR},            // arg 1
    {ASGN(T, T), STMT(T, T), BODY(T, T), EOAR}, // arg 2
    {{SYM_BRA, TOKENL, F, F}, EOAR},            // arg 3
};

node new_assgn(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}
node new_fcall(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}
node new_nsdef(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}
node new_exprs(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}
node new_sttmt(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}
node new_value(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}

node new_sbody(u64 i, lexout * tkns){
    node out = {0, 0, 0};
    return out;
}

ast parse(lexout * tkns){
    ast output;
    output.arr = malloc(tkns->tknc * sizeof(node));

    rule  * queue = malloc(tkns->tknc * sizeof(rule));
    i64     q_ptr = 0;
    bool mtch = T;

    // first push all tokens as nodes to queue
    void * tktps[] = {
        &&case_KEYWORD ,
        &&case_INDEXER ,
        &&case_LITERAL ,
        &&case_LSYMBOL ,
        &&case_OPERATOR,
    };
    #define JMP if(q_ptr < tkns->tknc - 1)\
    goto *tktps[tkns->tkns[q_ptr].type];\
    else goto out;

    JMP;
    case_KEYWORD :{
        queue[q_ptr].vall = tkns->tkns[q_ptr].vall.num;
        queue[q_ptr].type = KEYWRD;
        q_ptr++;
        JMP;
    }
    case_INDEXER :{
        queue[q_ptr].vall = q_ptr;
        queue[q_ptr].type = TOKENL;
        q_ptr++;
        JMP;
    }
    case_LITERAL :{
        queue[q_ptr].vall = q_ptr;
        queue[q_ptr].type = TOKENL;
        q_ptr++;
        JMP;
    }
    case_LSYMBOL :{
        queue[q_ptr].vall = tkns->tkns[q_ptr].vall.num;
        queue[q_ptr].type = SYMBOL;
        q_ptr++;
        JMP;
    }
    case_OPERATOR:{
        queue[q_ptr].vall = tkns->tkns[q_ptr].vall.num;
        queue[q_ptr].type = OPERTR;
        q_ptr++;
        JMP;
    }
    out:
    void * jlbl[] = {
        &&case_ASSGN,
        &&case_FCALL,
        &&case_NSDEF,
        &&case_EXPRS,
        &&case_STTMT,
        &&case_SBODY,
        &&case_VALUE,
        &&case_NONE,
    };
    rule * rules_itmi[] = {
        assgn[0],
        fcall[0],
        nsdef[0],
        exprs[0],
        sttmt[0],
        sbody[0],
    };

    #define CLEAR_QUEUE(q, i, f) for(u64 n = i; n <= f; n++) q[n] = EOAR;

    while(mtch){
        mtch = F;
        u64 i, c;

        u16 rule_itm = 0;
        
        goto case_NONE;
        case_ASSGN:{
            // assigment ran out of items
            if(rule_itm == 3){
                // clear queue items no long used anymore
                CLEAR_QUEUE(queue, q_ptr - rule_itm + 1, q_ptr);
                q_ptr -= rule_itm - 1;

                queue[q_ptr].vall = ASSGN;
                queue[q_ptr].type = FLANGT;

                queue[q_ptr].cpos[0] = i;
                queue[q_ptr].cpos[1] = c;
                q_ptr++;

                printf("there is an assignment at %d:%d. yay!\n", i, c);
                goto case_NONE;
            }
            else if(!rule_itm) i = 0;

            u16 itm = 0;
            // check each item in the rule and see if the queue path matches
            while(assgn[rule_itm][itm].vall != -1){
                // the item matches
                if( queue[c].type == assgn[rule_itm][itm].type
                and queue[c].vall == assgn[rule_itm][itm].vall){
                    c++;
                    if(!assgn[rule_itm][itm].isvarious) rule_itm++;
                    goto case_ASSGN;
                }
                // the item doesn't match, but it's optional
                else if(assgn[rule_itm][itm].skippable){
                    rule_itm++;
                    goto case_ASSGN;
                }
                itm++;
            }
        }
        case_FCALL:{
            // assigment ran out of items
            if(rule_itm == 4){
                // clear queue items no long used anymore
                CLEAR_QUEUE(queue, q_ptr - rule_itm + 1, q_ptr);
                q_ptr -= rule_itm - 1;

                queue[q_ptr].vall = FCALL;
                queue[q_ptr].type = FLANGT;

                queue[q_ptr].cpos[0] = i;
                queue[q_ptr].cpos[1] = c;
                q_ptr++;
                printf("there is a function call at %d:%d. yay!\n", i, c);
                goto case_NONE;
            }
            else if(!rule_itm) i = 0;

            u16 itm = 0;
            // check each item in the rule and see if the queue path matches
            while(fcall[rule_itm][itm].vall != -1){
                // the item matches
                if( queue[c].type == fcall[rule_itm][itm].type
                and queue[c].vall == fcall[rule_itm][itm].vall){
                    c++;
                    if(!fcall[rule_itm][itm].isvarious) rule_itm++;
                    goto case_FCALL;
                }
                // the item doesn't match, but it's optional
                else if(fcall[rule_itm][itm].skippable){
                    rule_itm++;
                    goto case_FCALL;
                }
                itm++;
            }
        }
        case_NSDEF:{
            // assigment ran out of items
            if(rule_itm == 2){
                // clear queue items no long used anymore
                CLEAR_QUEUE(queue, q_ptr - rule_itm + 1, q_ptr);
                q_ptr -= rule_itm - 1;

                queue[q_ptr].vall = NSDEF;
                queue[q_ptr].type = FLANGT;

                queue[q_ptr].cpos[0] = i;
                queue[q_ptr].cpos[1] = c;
                q_ptr++;
                printf("there is a namespace definition at %d:%d. yay!\n", i, c);
                goto case_NONE;
            }
            else if(!rule_itm) i = 0;

            u16 itm = 0;
            // check each item in the rule and see if the queue path matches
            while(nsdef[rule_itm][itm].vall != -1){
                // the item matches
                if( queue[c].type == nsdef[rule_itm][itm].type
                and queue[c].vall == nsdef[rule_itm][itm].vall){
                    c++;
                    if(!nsdef[rule_itm][itm].isvarious) rule_itm++;
                    goto case_NSDEF;
                }
                // the item doesn't match, but it's optional
                else if(nsdef[rule_itm][itm].skippable){
                    rule_itm++;
                    goto case_NSDEF;
                }
                itm++;
            }
        }
        case_EXPRS:{

        }
        case_STTMT:{

        }
        case_VALUE:{

        }
        case_SBODY:{

        }
        // case NONE checks if any node
        // matches the first node of an
        // rule
        case_NONE: {
            for(u64 n = c; n < q_ptr; n++){
                u16 node_itm = 0;
                // for each first node of each rule do:
                for(u16 r = 0; r < NONE - 1; r++){
                    // if it's not the EOAR node
                    while((rules_itmi[r])[node_itm].vall != -1){
                        // check if node item type matches rule item type
                        if(queue[n].type == (rules_itmi[r])[node_itm].type){
                            c = n + 1;
                            rule_itm = 0;
                            
                            if(queue[n].type == KEYWRD)
                            printf("jumping to %d [%s key]\n", r, KEYWORDS[queue[n].vall]);

                            if(queue[n].type == SYMBOL)
                            printf("jumping to %d [%s sym]\n", r, SYMBOLS[queue[n].vall]);

                            if(queue[n].type == OPERTR)
                            printf("jumping to %d [%d opr]\n", r, OPERATORS[queue[n].vall]);

                            if(queue[n].type == TOKENL)
                            printf("jumping to %d [%s idx]\n", r, tkns->tkns[queue[n].vall].vall.str);

                            goto *jlbl[r];
                        }
                        node_itm++;
                    }
                }

            }
            goto exit;
        }
        exit:
    }
    free(queue);
    return output;
}