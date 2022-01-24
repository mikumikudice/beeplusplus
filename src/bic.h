#ifndef cmmn
    #include "cmmn.h"
#endif

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "elif" , "else" ,
    "for"  , "goto" , "break",
    "extrn", "auto" , "char" , "pntr",
    "dist" ,

    "length", "typeof", "sizeof",
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

typedef struct string_array {
    char ** arr;
    u64     len;
} charr;

#define carr_push(a, v) \
(a.arr[a.len++] = v, \
a.arr = realloc(a.arr, (a.len + 1) * sizeof(char *)))

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
    tkn *next,*last;
};
// apndx data
enum {
    FREEABLE = 1, STRING_L
};

// when tkn::next points to EOTT the token tree
// is over. EOTT also holds the first token
tkn EOTT = {};

/* formal Language rules         *\
*  =============================  *
*  Represents an evaluated node.  *
*  It starts at token_t::UNKNOWN  *
*  because the token after being  *
*  lexically analized will never  *
*  be of the type UNKNOWN, so we  *
\* can use this value again.     */
typedef enum formal_lang_rule {
    CONSTD = UNKNOWN, // constant definition
    DEFINE,  ASSIGN , // variable definition
    ARRDEF,  STTDEF , // objects  definition
    ENUMDF,  STRUCT , // typedef rules
    STTMNT,  EXPRSS , // evaluatable bodies
    LABELD,  JMPSTT , // goto statements
    FUNDEF,  FNCALL , // functions
    BODYDF,           // code scope definition
} rule_t;

typedef struct ast_node {
    tkn * code;   // rule pointer. Tells where the parsing is
    i16 * path;   // code path represented as types (rule_t + token_t)
    i16   path_t; // the node type
} node;

typedef struct ast_t {
    node * ctxt; // ast context
    u64    clen; // context length
} astt;

#define grow_ast(ctxt) \
ctxt.ctxt = realloc(ctxt.ctxt, ((ctxt.clen++) + 1) * sizeof(node))

// compiler
void comp(FILE * fptr, char * outf, char * lddf, char * mthd);
// lexer
tkn * lexit();

// parser rules
tkn *constd_r(tkn * c, astt * ctxt);
tkn *define_r(tkn * c, astt * ctxt);
tkn *assign_r(tkn * c, astt * ctxt);

tkn *sttdef_r(tkn * c, astt * ctxt);
tkn *enumdf_r(tkn * c, astt * ctxt);
tkn *struct_r(tkn * c, astt * ctxt);
tkn *arrdef_r(tkn * c, astt * ctxt);

tkn *sttmnt_r(tkn * c, astt * ctxt);
tkn *exprss_r(tkn * c, astt * ctxt);

tkn *fundef_r(tkn * c, astt * ctxt);
tkn *fncall_r(tkn * c, astt * ctxt);

tkn *labeld_r(tkn * c, astt * ctxt);
tkn *jmpstt_r(tkn * c, astt * ctxt);

tkn *bodydf_r(tkn * c, astt * ctxt);

astt flushast(astt * ctxt, u64 i, u64 f);

// parser
astt parse(tkn * tkns);

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
void assert(bool check, imut char * msg);
void cmperr(imut char * err, tkn * arw, tkn * cmpl);
void wrning(imut char * wrn, tkn * arw, tkn * cmpl);

#define BIC
#include "bic.c"
#include "lexer.c"
#include "parser.c"