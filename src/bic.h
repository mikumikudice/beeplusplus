#ifndef cmmn
    #include "cmmn.h"
#endif

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "for"  ,
    "else" , "auto" , "case" , "goto" ,
    "null" , "getc" , "putc" ,
    "extrn", "while", "break",

    "switch", "return", "struct", "default"
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
    // bitwise operators
    "|" , "||" , "=|" , "=||",
    "&" , "&&" , "=&" , "=&&",
    "^" , "=^" ,
    "~" , "=~" ,
    ">>", "=>>",
    "<<", "=<<",
};

typedef struct char_array {
    char ** arr;
    u64     len;
} carr;

#define carr_push(a, v) \
(a.arr[a.len++] = v, \
a.arr = realloc(a.arr, (a.len + 1) * sizeof(char *)))

typedef struct pattern {
    char * s, * e;
    bool(*func)(char *);
} ptrn;

bool vld_args(char * path);
bool vld_expr(char * path);
bool vld_scop(char * path);

// symbols
imut ptrn SYMBOLS[] = {
    (ptrn){.s = "(" , .e = ")" , .func = vld_args}, // call/exp
    (ptrn){.s = "{" , .e = "}" , .func = vld_scop}, // scope
    (ptrn){.s = "[" , .e = "]" , .func = vld_expr}, // indexing
    (ptrn){.s = nil , .e = "." , .func = nil     }, // dots
    (ptrn){.s = nil , .e = "," , .func = nil     }, // commas
    (ptrn){.s = nil , .e = ";" , .func = nil     }, // newline
    (ptrn){.s = nil , .e = "//", .func = nil     }, // comments
    (ptrn){.s = "/*", .e = "*/", .func = nil     }, // ...
};

#define SYM_PAR 0 // parentheses
#define SYM_BRA 1 // brackets
#define SYM_SQR 2 // square brackets
#define SYM_DOT 3 // dots
#define SYM_COM 4 // commas
#define SYM_EOL 5 // end of line
#define SYM_CMM 6 // comment
#define SYM_MLC 7 // multi-line comment

// hash table
// =====================
// a vector of key-value
// system for reading it
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
    (hash){.key = "**" , .val = "*" },
};

typedef enum token_type {
    UNKNOWN ,
    KEYWORD ,
    INDEXER ,
    LITERAL ,
    LSYMBOL ,
    OPERATOR,
} tknt;

// token
// ===========================
// a token that holds a string
// value and its function role
typedef struct token {
    char * vall;
    tknt   type;
    size_t line, coll;
} token;

// lexer output
// ==============================
// a dynamic array containing all
// the spited tokens by the lexer
typedef struct lexer_out {
    token * tkns;
    size_t  tknc;
} lexout;

#define tkn_push(arr, val) \
(arr.tkns[arr.tknc++] = val, \
arr.tkns = realloc(arr.tkns, (arr.tknc + 1) * sizeof(token)))

// compiler
void comp(FILE * fptr, char * outf, char * lddf, char * mthd);
// lexer
lexout lexit();

bool isnumc(char chr);
bool ishexc(char chr);
bool validn(char chr);

bool matchs(char * str, bool(*func)(char));
bool hassym(char * str);
bool isscpd(char * str, u64 chr);
i16  iskeyw(char * str);

char * hextostr(char * str);
char * strtohex(char * str);
char * strtoptr(char * str);

// compilation error
void cmperr(imut char * err, token * arw, void * cmpl);

#include "bic.c"