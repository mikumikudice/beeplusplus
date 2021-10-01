#ifndef BIC
    #include "bic.h"
#endif

/* parsing code                                 *\
*  ============================================  *
*  the parser tries to match the code path with  *
*  a parsing rule. If the current token matches  *
*  the rule, the ruler updates the context node  *
*  and calls itself once again, now pointing to  *
*  the next token. If the rule check fails, the  *
*  ruler will return nil to the last caller and  *
*  unwind the stack frame until reach the first  *
*  caller, the "parse" function, that now gonna  *
*  try another rule. If the rule check ran with  *
*  no problems the ast will be updated and then  *
*  the parser will check the next code path. If  *
*  in the end there is alone tokens, the parser  *
*  will rise an syntax error.                    *
\*                                              */

// code path rules
node * constd_r(tkn * c, node * ctxt){
    return ctxt;
}
node * define_r(tkn * c, node * ctxt){
    return ctxt;
}
node * assign_r(tkn * c, node * ctxt){
    return ctxt;
}
node * arrdef_r(tkn * c, node * ctxt){
    return ctxt;
}
node * sttdef_r(tkn * c, node * ctxt){
    return ctxt;
}
node * enumdf_r(tkn * c, node * ctxt){
    return ctxt;
}
node * struct_r(tkn * c, node * ctxt){
    return ctxt;
}
node * sttmnt_r(tkn * c, node * ctxt){
    return ctxt;
}
node * exprss_r(tkn * c, node * ctxt){
    return ctxt;
}
node * labeld_r(tkn * c, node * ctxt){
    return ctxt;
}
node * jmpstt_r(tkn * c, node * ctxt){
    return ctxt;
}
node * fundef_r(tkn * c, node * ctxt){
    return ctxt;
}
node * fncall_r(tkn * c, node * ctxt){
    return ctxt;
}

charr parse(tkn * tkns){
    // if any rule breaks, retreat parsing until here
    tkn * safe = tkns;

}