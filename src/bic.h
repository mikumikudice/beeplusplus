#ifndef cmmn
    #include "cmmn.h"
#endif

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "if"   , "else" , "case" , "auto" ,
    "for"  , "while", "goto" , "break",
    "getc" , "putc" , "print", "flush",
    "array", "value", "func" , "extrn",
    "alloc", "free" , "nil"  ,

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
    (hash){.key = "*e" , .val = "\0"},
    (hash){.key = "**" , .val =  "*"},
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
// ================================
// a struct that holds a the string
// literal, its syntax role and its
// code positioning (line and coll)
typedef struct token {
    char * vall;
    tknt   type;
    size_t line, coll; // I know it's column but coll fits better
} token;

// lexer output
// ==============================
// a dynamic array containing all
// the tokens spited by the lexer
typedef struct lexer_out {
    token * tkns;
    size_t  tknc;
} lexout;

// AST
// ========================================
// an Abstract Syntax Tree holds all tokens
// in its place and information about their
// function, according to the meaning given
// to it and its position in the statement.
// AST objects are ACDESV encoded, what is,
// each node is either an Assigment, a func
// Call, a namespace Definition (which also
// may hold an assigment within it), a math
// Expression, a Statement or a Variable or
// literal Value.

// layout: [ASSGN <XXXX> <YYYY> <ZZZZ>]
// XXXX: assignment ip
// YYYY: scope level
// ZZZZ: namespaces defined
char * new_assgn(u64 i, lexout * tkns);

// layout: [FCALL <XXXX> <YYYY> <ZZZZ>]
// XXXX: function name
// YYYY: function args
// ZZZZ: returned kind
char * new_fcall(u64 i, lexout * tkns);

// layout: [NSDEF <XXXX> <YYYY>]
// XXXX: readability
// YYYY: assignment/namespaces
char * new_nsdef(u64 i, lexout * tkns);

// layout: [EXPRS <XXXX> <YYYY> <ZZZZ>]
// XXXX: lvalue
// YYYY: operation
// ZZZZ: rvalue
char * new_exprs(u64 i, lexout * tkns);

// layout: [STTMT <XXXX> <YYYY> <ZZZZ>]

// when XXXX is a keyword:
// YYYY: variable definition
// ZZZZ: body

// when XXXX is "BODY"
// YYYY: AST node ip
// ZZZZ: statement return kind
char * new_sttmt(u64 i, lexout * tkns);

// layout: [VALUE <XXXX> <YYYY>]
// XXXX: token tree ip
// YYYY: kind + "LITERAL"
char * new_value(u64 i, lexout * tkns);

#define tkn_push(arr, val) \
(arr.tkns[arr.tknc++] = val, \
arr.tkns = realloc(arr.tkns, (arr.tknc + 1) * sizeof(token)))

// compiler
void comp(FILE * fptr, char * outf, char * lddf, char * mthd);
// lexer
lexout lexit();
// parser
charr   parse();

bool isnumc(char chr);
bool ishexc(char chr);
bool validn(char chr);

bool matchs(char * str, bool(*func)(char));
bool hassym(char * str);
bool isscpd(char * str, u64 chr);
i16  iskeyw(char * str);

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