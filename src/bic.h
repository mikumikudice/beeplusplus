#ifndef cmmn
    #include "cmmn.h"
#endif

#define assert(check, msg)\
    if(!(check)){\
        fprintf(stderr, RED);\
        fprintf(stderr, "[ERROR]");\
        fprintf(stderr, DEF);\
        if(msg) fprintf(stderr, ": %s at %d in %s\n", msg, __LINE__, __FILE__);\
        else fprintf(stderr, ": %s failed at %d in %s\n", #check, __LINE__, __FILE__);\
        exit(-1);\
    }

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "elif" , "else" ,
    "for"  , "goto" , "break",
    "extrn", "auto" , "char" , "pntr",
    "dist" , "from" ,

    "typeof", "sizeof", "getval",
    "struct", "switch", "return",
};
enum {
    KW_IF   , KW_ELIF , KW_ELSE ,
    KW_FOR  , KW_GOTO , KW_BREAK,
    KW_EXTRN, KW_AUTO , KW_CHAR , KW_PNTR,
    KW_DIST ,

    KW_LENGTH, KW_KINDOF, KW_SIZEOF,
    KW_STRUCT, KW_SWITCH, KW_RETURN,
};

// list of operators
imut char OPERATORS[][4] = {
    // equality and assignment operators
    "="  , "==" ,
    "!"  , "!=" ,
    "<"  , "<=" ,
    ">"  , ">=" ,

    // arithmetic operators
    "+"  , "+=" ,
    "-"  , "-=" ,
    "*"  , "*=" ,
    "/"  , "/=" ,
    "%"  , "%=" ,

    // boolean operators
    "not", "and", "or",

    // bitwise operators
    "~"  , "~=" , // not / xor
    "|"  , "|=" , // or
    "&"  , "&=" , // and / adr
    ">>" , ">>=", // left shift
    "<<" , "<<=", // right shift

    // miscellaneous
    "^"  ,        // pointer operator
    ":"           // label operator
};

// is this operator unary?
#define is_unr_opr(opr)\
(opr == 2 or opr == 8 or opr == 10 or opr == 21 or opr == 25 or opr == 31)

// is this an assignment operator?
#define is_ass_opr(opr)\
(opr == 0 or (opr % 2 == 1 and opr < 18 and opr > 7) or (opr % 2 == 0 and opr > 20))

// is this an equality operator?
#define is_eql_opr(opr)\
(opr % 2 == 1 and opr < 8)

// is this operator boolean?
#define is_bln_opr(opr)\
(opr > 17 and opr < 21)

typedef struct string_array {
    char ** arr;
    u64     len;
} stra;

#define stra_push(a, v) \
(a.arr[a.len++] = v, \
a.arr = realloc(a.arr, (a.len + 1) * sizeof(char *)))

i64 find_itm(stra * a, char * i);
i64 find_itm(stra * a, char * i){
    for(u64 t = 0; t < a->len; t++){
        if(!strcmp(a->arr[t], i)) return t;
    }
    return -1;
}

typedef struct pattern {
    char * s, * e;
} ptrn;

// symbols
imut ptrn SYMBOLS[] = {
    (ptrn){.s = "(" , .e = ")" }, // call/exp
    (ptrn){.s = "{" , .e = "}" }, // scope
    (ptrn){.s = "[" , .e = "]" }, // indexing
    (ptrn){.s = nil , .e = "." }, // dot
    (ptrn){.s = nil , .e = "," }, // comma
    (ptrn){.s = nil , .e = ";" }, // semicolon
    (ptrn){.s = nil , .e = "//"}, // comments
    (ptrn){.s = "/*", .e = "*/"}, // ...
};
enum {
    SYM_PAR, // parentheses
    SYM_BRA, // brackets
    SYM_SQR, // square brackets
    SYM_DOT, // dots
    SYM_COM, // commas
    SYM_CLN, // semicolon
    SYM_CMM, // comment
    SYM_MLC, // multi-line comment
};

/* hash table                 *\
*  ==========================  *
*  is vector with a key-value  *
\* system to read its content */
typedef struct hash_table {
    char * val;
    char * key;
} hash;

// list of string metachars
imut hash metachar[] = {
    (hash){.key = "*'" , .val = "\'"},
    (hash){.key = "*\"", .val = "\""},
    (hash){.key = "*n" , .val = "\n"},
    (hash){.key = "*t" , .val = "\t"},
    (hash){.key = "*r" , .val = "\r"},
    (hash){.key = "*b" , .val = "\b"},
    (hash){.key = "*e" , .val = "\0"},
    (hash){.key = "**" , .val =  "*"},
};

typedef enum token_t {
    KEYWORD, INDEXER ,
    LITERAL,
    LSYMBOL, OPERATOR,
    UNKNOWN,
} tknt;

/* token                            *\
*  ================================  *
*  a struct that holds a the string  *
*  literal, its syntax role and its  *
\* code positioning (line and coll) */

typedef union string_or_int {
    char * str; // string for literals and indexes
    u16    num; // number to keywords and operators (pointers)
} aori;

typedef struct token tkn;
struct token {
    u32  apdx;
    u32  data;
    aori vall;
    tknt type;
    u64  line, coll; // I know it's column but coll fits better
    u64  tabl;
    tkn *next,*last;
};
// apndx data
enum {
    FREEABLE = 1, STRING_L
};

// when tkn::next points to EOTT the token tree
// is over. EOTT also holds the first token
tkn __EOTT = {};
tkn * EOTT = &__EOTT;

/* formal Language rules         *\
*  =============================  *
*  Represents an evaluated node.  *
*  It starts at token_t::UNKNOWN  *
*  because the token after being  *
*  lexically analized will never  *
*  be of the type UNKNOWN, so we  *
\* can use this value again.     */
typedef enum formal_lang_rule {
    CONSTD = UNKNOWN,         // constant definition
    DEFINE,  ASSIGN ,         // variable definition
    ARRDEF,  STTDEF ,         // objects  definition
    ENUMDF,  STRUCT ,         // typedef rules
    STTMNT,  EXPRSS ,         // evaluatable bodies
    LABELD,  JMPSTT ,         // goto statements
    FUNDEF,  FNCALL , ARGDEF, // functions
    BODYDF,                   // code scope definition
} rule_t;

typedef struct ast_node {
    tkn * strt;   // code path pointer
    tkn * end_of; // end of code path
    u16 * path;   // code path represented as types (rule_t + token_t)
    u16   path_t; // the node type
} node;

/* ruler output                *\
*  ===========================  *
*  `node` holds the code path,  *
*  and `end` points to the end  *
*  of the evaluated code path.  *
*  when no node is needed it's  *
*  cheaper send just the token  *
*  as a pointer rather than an  *
*  entire node just for it.     *
\*                             */
typedef struct ruler_out {
    node * out;
    tkn  * end;
} rulr;

typedef struct ast_t {
    node **ctxt; // ast context
    u64    clen; // context length
} astt;

#define grow_ast(ctxt) \
    ctxt->ctxt = realloc(ctxt->ctxt, ((ctxt->clen++) + 1) * sizeof(node))

#define pushnode(p, c, t, ctxt)\
    ctxt->ctxt[ctxt->clen].strt = p;\
    ctxt->ctxt[ctxt->clen].path = c;\
    ctxt->ctxt[ctxt->clen].path_t = t;\
    grow_ast(ctxt)


/* compiler output                       *\
*  =====================================  *
*  holds the output name and all defined  *
*  global-scopped namespaces.             *
\*                                       */
typedef struct cout {
    char *  outn;
    char *  nasm;

    char ** defn;
    u64     defc;

    char ** extr;
    u64     extc;
} cout;

typedef enum comp_mode {
    CHECK, DEBUG, BUILD
} cmod;

/* compiler                      *\
*  =============================  *
*  compiles the target file into  *
*  a NASM file (as a string).     *
\*                               */
cout * comp(FILE * fptr, char * lddf, char * mode);
// lexer
tkn * lexit();

char * get_tokval(tkn * tok);

tkn * matchpair(tkn * c);
void  hasscolon(rulr out);

// parser rules
rulr constd_r(tkn * c, bool jchk);
rulr define_r(tkn * c, bool jchk);
rulr assign_r(tkn * c, bool jchk);

rulr sttdef_r(tkn * c, bool jchk);
rulr enumdf_r(tkn * c, bool jchk);
rulr struct_r(tkn * c, bool jchk);
rulr arrdef_r(tkn * c, bool jchk);

rulr sttmnt_r(tkn * c, bool jchk);
rulr exprss_r(tkn * c, bool jchk, bool prnd);

rulr fun_def_r(tkn * c, bool jchk);
rulr funcall_r(tkn * c, bool jchk);

rulr labeldf_r(tkn * c, bool jchk);
rulr jmp_stt_r(tkn * c, bool jchk);

rulr bodydef_r(tkn * c, bool jchk);

astt *flush_ast(astt * ctxt, u64 i, u64 f);

// parser
astt parse(tkn * tkns, cmod mode);

bool isnumc(char chr);
bool ishexc(char chr);
bool validn(char chr);

bool matchs(char * str, bool(*func)(char));
bool hassym(char * str);
bool isscpd(char * str, u64 chr);
i16  iskeyw(char * str);

u64 upow(u64 b, u64 p);

//char * hextostr(char * str);
char * strtohex(char * str);
u16    strtoptr(char * str);
void   free_str();

// compilation error
void cmperr(imut char * err, tkn * arw, tkn * cmpl);
void wrning(imut char * wrn, tkn * arw, tkn * cmpl);

#define BIC
#include "bic.c"
#include "lexer.c"
#include "parser.c"