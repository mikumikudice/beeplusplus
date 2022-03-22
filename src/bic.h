#ifndef cmmn
    #include "cmmn.h"
#endif

#define assert(check, msg)\
    if(!(check)){\
        fprintf(stderr, RED);\
        fprintf(stderr, "[ERROR]:");\
        fprintf(stderr, DEF);\
        if(msg) fprintf(stderr, " `%s` falied! %s at %s#%d\n", #check, msg, __FILE__, __LINE__);\
        else fprintf(stderr, " `%s` failed at %s#%d\n", #check, __FILE__, __LINE__);\
        exit(-1);\
    }

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "elif" , "else" ,
    "for"  , "goto" , "break", "next",
    "extrn", "auto" , "char" , "pntr",
    "dist" , "from" , "in"   ,

    "typeof", "sizeof", "getval",
    "struct", "switch", "return",
};
enum {
    KW_IF   , KW_ELIF , KW_ELSE ,
    KW_FOR  , KW_GOTO , KW_BREAK,
    KW_EXTRN, KW_AUTO , KW_CHAR , KW_PNTR,
    KW_DIST ,

    KW_TYPEOF, KW_SIZEOF, KW_GETVAL,
    KW_STRUCT, KW_SWITCH, KW_RETURN,
};
u16 ldef[2] = {07,  9}; // local var's definition keywords
u16 sttt[2] = {00, 03}; // statement keywords
u16 funl[2] = {11, 13}; // function-like keywords

// list of operators
imut char OPERATORS[][4] = {
    // assignment operators
    "="  , "+=", "-=", "*=" , "/=" , "%=",
    "~=" , "|=", "&=", ">>=", "<<=",
    // equality operators
    "==" , "!=", "<" , ">"  , "<=" , ">=",

    // arithmetic operators
    "*"  , "/" , "%" , "+"  , "-"  ,
    // bitwise operators
    "~"  , "|" , "&" , ">>" , "<<" ,

    // boolean operators
    "and", "or", "not",

    // miscellaneous
    "^"  ,        // pointer operator
    ":"  ,        // label operator
    ".."          // range operator
};

imut u16 OPR_LEN = arrlen(OPERATORS);
u16 asgn[2] = {0 , 10}; // assignment operators
u16 eqlt[2] = {11, 16}; // equality operators
u16 arth[2] = {17, 21}; // arithmetic operators
u16 unry[2] = {20, 22}; // unary operators
u16 btws[2] = {22, 26}; // bitwise operators
u16 blan[2] = {27, 29}; // boolean operators

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
    (ptrn){.s = "." , .e = nil }, // dot
    (ptrn){.s = "," , .e = nil }, // comma
    (ptrn){.s = ";" , .e = nil }, // semicolon
    (ptrn){.s = "//", .e = nil }, // comments
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
    char * str; // string for literals and indexers
    u16    num; // number to keywords and operators (pointers)
} aori;

typedef struct token tkn;
struct token {
    u32  apdx;
    u32  data;
    aori vall;
    tknt type;
    u64  line, coll; // I know it's column but coll fits better
    tkn *next,*last;
};
// apndx data
enum {
    FREEABLE = 1, STRING_L
};

// when tkn::next points to EOTT the token tree
// is over. EOTT also holds the first token
tkn __EOTT = {.type = UNKNOWN};
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
    CODEIS                    // representative value (code itself)
} rule_t;

/* Abstract Syntax Tree node            *\
*  ====================================  *
*  this type holds the entire code as a  *
*  linked list of nodes. Each node is a  *
*  hotpath like a statement or variable  *
*  assignment. It was implemented as so  *
*  because it's easier to add or remove  *
*  nodes from it without reallocing it.  *
\*                                      */
typedef struct ast_node node;
struct ast_node {
    node *last, *next;
    u16   vall;
    aori  itsf;
};
typedef struct ast_t astt;

struct ast_t {
    tkn * strt;        // code path pointer
    tkn * endo;        // end of code path
    node *pstt, *pend; // the code path
};

// when node::next points to EOAST the AST is
// over. EOAST also holds the first AST node.
astt __EOAST = {};
astt * EOAST = &__EOAST;

#define grow_ast(ctxt) \
    ctxt->ctxt = realloc(ctxt->ctxt, ((ctxt->clen++) + 1) * sizeof(astt))

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
    CHECK, DEBUG, BUILD, SCOPE
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
void  hasscolon(astt * out);

// parser rules
astt * constd_r(tkn * c);
astt * define_r(tkn * c);
astt * assign_r(tkn * c, bool prnd);
astt * strdef_r(tkn * c);
astt * enumdf_r(tkn * c);
astt * struct_r(tkn * c);
astt * arrdef_r(tkn * c);
astt * sttmnt_r(tkn * c);
astt * exprss_r(tkn * c, bool prnd);
astt *fun_def_r(tkn * c);
astt *funcall_r(tkn * c);
astt *labeldf_r(tkn * c);
astt *jmp_stt_r(tkn * c);
astt *bodydef_r(tkn * c);

// parser
astt * parse(tkn * tkns, cmod mode);

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

char * nodet_to_str(node * n);

// compilation error
void cmperr(imut char * err, tkn * arw, tkn * cmpl);
void wrning(imut char * wrn, tkn * arw, tkn * cmpl);

#define BIC
#include "bic.c"
#include "lexer.c"
#include "parser.c"