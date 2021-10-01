#ifndef cmmn
    #include "cmmn.h"
#endif

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "else" ,
    "for"  , "goto" , "break",
    "array", "value", "func" , "enum" ,
    "extrn", "auto" , "dist" , "nil"  ,

    "length", "kindof", "sizeof",
    "struct", "switch", "return",
};
enum {
    KW_IF   , KW_ELSE ,
    KW_FOR  , KW_GOTO , KW_BREAK,
    KW_ARRAY, KW_VALUE, KW_FUNC , KW_ENUM ,
    KW_EXTRN, KW_AUTO , KW_DIST , KW_NIL  ,

    KW_LENGTH, KW_KINDOF, KW_SIZEOF,
    KW_STRUCT, KW_SWITCH, KW_RETURN,
};

// list of operators
imut char OPERATORS[][4] = {
    // equality operators
    "=" , "==" , "===",
    "!" , "!=" , "=!=",
    "<" , "=<" ,
    "<=", "=<=",
    ">" , "=>" ,
    ">=", "=>=",
    // arithmetic operators
    "+" , "++" , "+=",
    "-" , "--" , "-=",
    "*" , "*=" ,
    "/" , "/=" ,
    "%" , "%=" ,
    // boolean operators
    "and", "or",
    "|" , "||" , "=|" , "=||",
    "&" , "&&" , "=&" , "=&&",
    // bitwise operators
    "^" , "=^" ,
    "~" , "=~" ,
    ">>", "=>>",
    "<<", "=<<",
    // label operator
    ":"
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
    (ptrn){.s = nil , .e = "." }, // dots
    (ptrn){.s = nil , .e = "," }, // commas
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

// hash table
// ==========================
// a vector with a key-value
// system to read its content
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

// token
// ================================
// a struct that holds a the string
// literal, its syntax role and its
// code positioning (line and coll)

typedef union string_or_int {
    char * str; // string for literals and indexes
    u16    num; // number to keywords and operators (pointers)
} aori;

typedef struct token tkn;
struct token {
    u32  apdx;
    aori vall;
    tknt type;
    u64  line, coll; // I know it's column but coll fits better

    // points to the next token
    tkn *next;
};

// When tkn::next points to EOFT the token tree is over
imut tkn EOFT = {};

// formal Language rules
// =============================
// Represents an evaluated node.
// It starts at token_t::UNKNOWN
// because the token after being
// lexically analized will never
// be of the type UNKNOWN, so we
// can use this value again.
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
    u16 * path; // code path represented as types (rule_t + token_t)
    u16 rule_p; // rule pointer. Tells where the parsing is
} node;

// compiler
void comp(FILE * fptr, char * outf, char * lddf, char * mthd);
// lexer
tkn * lexit();

node * constd_r(tkn * c, node * ctxt);
node * define_r(tkn * c, node * ctxt);
node * assign_r(tkn * c, node * ctxt);
node * arrdef_r(tkn * c, node * ctxt);
node * sttdef_r(tkn * c, node * ctxt);
node * enumdf_r(tkn * c, node * ctxt);
node * struct_r(tkn * c, node * ctxt);
node * sttmnt_r(tkn * c, node * ctxt);
node * exprss_r(tkn * c, node * ctxt);
node * labeld_r(tkn * c, node * ctxt);
node * jmpstt_r(tkn * c, node * ctxt);
node * fundef_r(tkn * c, node * ctxt);
node * fncall_r(tkn * c, node * ctxt);

// parser
charr parse(tkn * tkns);

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
char * strtoptr(char * str);

// compilation error
void assert(bool check, imut char * msg);
void cmperr(imut char * err, tkn * arw, tkn * cmpl);
void wrning(imut char * wrn, tkn * arw, tkn * cmpl);

#define BIC
#include "bic.c"
#include "lexer.c"
#include "parser.c"