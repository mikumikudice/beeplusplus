#ifndef cmmn
    #include "cmmn.h"
#endif

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "else" ,
    "for"  , "while", "goto" , "break",
    "getc" , "putc" , "print", "flush",
    "array", "value", "func" , "enum" ,
    "extrn", "auto" , "dist" ,
    "alloc", "free" , "nil"  , "exit" ,

    "length", "kindof", "sizeof",
    "struct", "switch", "return",
    "default"
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

#define SYM_PAR 0 // parentheses
#define SYM_BRA 1 // brackets
#define SYM_SQR 2 // square brackets
#define SYM_DOT 3 // dots
#define SYM_COM 4 // commas
#define SYM_CLN 5 // semicolon
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
    (hash){.key = "*e" , .val = "\0"},
    (hash){.key = "**" , .val =  "*"},
};

typedef enum token_type {
    KEYWORD ,
    INDEXER ,
    LITERAL ,
    LSYMBOL ,
    OPERATOR,
    UNKNOWN ,
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

typedef struct token {
    aori vall;
    tknt type;
    u64  line, coll; // I know it's column but coll fits better
} token;

// lexer output
// ==============================
// a dynamic array containing all
// the tokens spited by the lexer
typedef struct lexer_out {
    token * tkns;
    size_t  tknc;
} lexout;

#define tkn_push(arr, val) \
(arr.tkns[arr.tknc++] = val, \
arr.tkns = realloc(arr.tkns, (arr.tknc + 1) * sizeof(token)))

imut char * nodes[] = {
    "0:%.07x",
    "1:%.07x",
    "2:%.07x",
    "2:%.07x",
    "3:%.07x",
};

// Formal Language rules
typedef enum ACDESV {
    DEFINE, ASSIGN, // variable definition
    CONSTD,         // constant definition
    ARRDEF, 
    UNIOND, STRUCT, // typedef rules
    STTMNT, EXPRSS, // evaluatable bodies
    LABELD, JMPSTT, // goto statements
    
} rule;

// compiler
void comp(FILE * fptr, char * outf, char * lddf, char * mthd);
// lexer
lexout lexit();
// parser
charr parse(lexout * tkns);

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
void cmperr(imut char * err, token * arw, token * cmpl);
void wrning(imut char * wrn, token * arw, token * cmpl);

#define BIC
#include "bic.c"
#include "lexer.c"
#include "parser.c"