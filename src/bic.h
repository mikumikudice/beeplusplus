#ifndef cmmn
    #include "cmmn.h"
#endif

#define assert(check, msg)\
    if(!(check)){\
        fprintf(stderr, RED);\
        fprintf(stderr, "[ERROR]:");\
        fprintf(stderr, DEF);\
        if(msg) fprintf(stderr, " `%s` failed! %s at %s:%d\n", #check, msg, __FILE__, __LINE__);\
        else fprintf(stderr, " `%s` failed at %s:%d\n", #check, __FILE__, __LINE__);\
        exit(-1);\
    }

// list of the reserved keywords
imut char KEYWORDS[][8] = {
    "for"   , "switch", "if"    , "elif",
    "else"  ,
    "goto"  , "extrn" , "return",
    "next"  , "break" ,
    "typeof", "sizeof", "getval",
    "struct", "enum"  ,
    "auto"  , "char"  , "pntr"  ,
    "dist"  , "from"  ,  "in"   , "as",
};
enum {
    KW_FOR , KW_SWITCH, KW_IF, KW_ELIF,   // true statements
    KW_ELSE,                              // body holder

    KW_GOTO, KW_EXTRN, KW_RETURN,         // expression holders
    KW_NEXT, KW_BREAK,                    // single ones

    KW_TYPEOF, KW_SIZEOF, KW_GETVAL ,     // function-like
    KW_STRUCT, KW_ENUM,                   // other statements

    KW_AUTO  , KW_CHAR, KW_PNTR,          // not statements
    KW_DIST  , KW_FROM, KW_IN  , KW_AS,
    
};

u16 sttt[2] = { 0,  9}; // statement keywords
u16 trus[2] = { 0,  3}; // true statements
u16 body[2] = { 4,  4}; // body holder statements
u16 hldr[2] = { 5,  7}; // expression holder statements
u16 sngl[2] = { 8,  9}; // single statements
u16 funl[2] = {10, 12}; // function-like statements
u16 othr[2] = {13, 14}; // other statements
u16 ldef[2] = {15, 17}; // local var's definition keywords
u16 iftl[2] = { 3,  4}; // if tail

// list of operators
imut char OPERATORS[][4] = {
    // assignment operators
    "="  , "+=", "-=", "*=" , "/=" , "%=",
    "~=" , "|=", "&=", "^=" , ">>=", "<<=",
    // equality operators
    "==" , "!=", "<" , ">"  , "<=" , ">=",

    // arithmetic operators
    "*"  , "/" , "%" , "+"  , "-"  ,
    // bitwise operators
    "~"  , "&" , "|" ,  "^" , ">>" , "<<" ,

    // miscellaneous
    ":"  ,        // label operator
    ".."          // range operator
};

imut char TXT_OPERS[][4] = {
    // boolean operators
    "and", "or", "not",
};

u16 asgn[2] = {00, 11}; // assignment operators
u16 eqlt[2] = {12, 17}; // equality operators
u16 arth[2] = {18, 22}; // arithmetic operators
u16 unry[2] = {21, 24}; // unary operators
u16 btws[2] = {23, 28}; // bitwise operators

u16 blns[2] = {00,  3}; // boolean operators

typedef struct string_array {
    char **arr;
    u64    len;
    u64  lgrst; // largest string's size
} stra;

#define stra_push(a, v) \
(a.arr[a.len++] = v, \
a.arr = realloc(a.arr, (a.len + 1) * sizeof(char *)))

i64 find_itm(stra *a, char *i);
i64 find_itm(stra *a, char *i){
    for(u64 t = 0; t < a->len; t++){
        if(!strcmp(a->arr[t], i)) return t;
    }
    return -1;
}

typedef struct pattern {
    char *s, *e;
} ptrn;

// symbols
imut ptrn SYMBOLS[] = {
    (ptrn){.s = "(" , .e = ")" }, // call/exp
    (ptrn){.s = "{" , .e = "}" }, // scope
    (ptrn){.s = "[" , .e = "]" }, // array [and] indexing
    (ptrn){.s = "." , .e = nil }, // dot
    (ptrn){.s = "," , .e = nil }, // comma
    (ptrn){.s = ";" , .e = nil }, // semicolon
    (ptrn){.s = "//", .e = nil }, // comments
    (ptrn){.s = "/*", .e = "*/"},
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
    char *val;
    char *key;
} hash;

// list of string metachars
imut hash metachar[] = {
    (hash){.key = "*'" , .val = "\'"},
    (hash){.key = "*\"", .val = "\""},
    (hash){.key = "*n" , .val = "\n"},
    (hash){.key = "*t" , .val = "\t"},
    (hash){.key = "*r" , .val = "\r"},
    (hash){.key = "*b" , .val = "\b"},
    (hash){.key = "**" , .val =  "*"},
};

typedef enum token_t {
    KEYWORD, INDEXER ,
    LITERAL,
    LSYMBOL, OPERATOR,
    UNKNOWN,
} tknt;

typedef union string_or_int {
    char *str; // string for literals and indexers
    u32   num; // number to keywords and operators (pointers)
} aori;

/* token                            *\
*  ================================  *
*  a struct that holds a the string  *
*  literal, its syntax role and its  *
\* code positioning (line and coln) */
typedef struct token tkn;
struct token {
    u32  apdx;
    u32  data;
    aori vall;
    tknt type;
    u64  line,  coln;
    tkn *next, *last;
};

enum apendix_value {
    NUMBER,
    DWCHAR,
    STRING,
};

// when tkn::next points to EOTT the token tree
// is over. EOTT also holds the first token and
// last tokens.
tkn _EOTT = {.type = UNKNOWN};
tkn *EOTT = &_EOTT;

/* formal Language rules         *\
*  =============================  *
*  Represents an evaluated node.  *
*  It starts at token_t::UNKNOWN  *
*  because the token after being  *
*  lexically analyzed will never  *
*  be of the type UNKNOWN, so we  *
\* can use this value again.     */
typedef enum formal_lang_rule {
    CONSTD = UNKNOWN,         // constant definition
    DEFINE,  ASSIGN ,         // variable definition
    ARRDEF,  STRDEF ,         // objects  definition
    ENUMDF,  STRUCT ,         // typedef rules
    STTMNT,  EXPRSS ,         // evaluatable bodies
    LABELD,  JMPSTT ,         // goto statements
    FUNDEF,  FNCALL ,         // functions
    PARAMT,  PPARAM ,         // types of function parameters
    IDXING,  ACCESS ,         // arrays and structures field access
    BODYDF,  EOSCPE ,         // code scope definition
    CODEIS,  CDHALT ,         // representative value (code itself)
    COLLEC,  MIDREP ,         // collection of nodes
} rule_t;

/* Abstract Syntax Tree node            *\
*  ====================================  *
*  this type holds the entire code as a  *
*  linked list of nodes. Each node is a  *
*  hotpath like a statement or variable  *
*  assignment. It was implemented as so  *
*  because it's easier to add or remove  *
*  nodes in a non linear way.            *
\*                                      */
typedef struct ast_node node;
struct ast_node {
    node *last, *next; // used for reading
    node *stt , *end;  // used for navigation
    tkn  *ftok, *ltok;
    u16   type;
    tkn  *vall;
    u64   dcnt;        // only used by paths like define or fncall
    bool  is_parent;
};


/* compiler output                       *\
*  =====================================  *
*  holds the output name and all defined  *
*  global-scopped namespaces.             *
\*                                       */
typedef struct cout {
    char  *outn; // output file name
    char  *nasm; // nasm code

    char **defn; // defined namespaces
    u64    defc; // dn count

    char **extr; // external requirements
    u64    extc; // er count
} cout;

typedef enum comp_mode {
    CHECK, DEBUG, BUILD, SCOPE
} cmod;

/* compiler                      *\
*  =============================  *
*  compiles the target file into  *
*  a NASM file (as a string).     *
\*                               */
cout *comp(FILE *fptr, char *lddf, char *mode);

// code loader
stra  load(FILE *fptr);

// lexer
tkn  *lexit();

char *get_tokval(tkn *tok);

tkn  *matchpair(tkn *c);
void  hasscolon(node *out);

// parser rules
node  *constd_r(tkn *c);
node  *define_r(tkn *c);
node  *assign_r(tkn *c, bool prnd);
node  *strdef_r(tkn *c);
node  *enumdf_r(tkn *c);
node  *struct_r(tkn *c);
node  *arrdef_r(tkn *c);
node  *sttmnt_r(tkn *c);
node  *exprss_r(tkn *c, bool prnd);
node *fun_def_r(tkn *c);
node *funcall_r(tkn *c);
node *labeldf_r(tkn *c, bool is_swedish);
node *jmp_stt_r(tkn *c);
node *extrn_exp(tkn *c);
node *validthnd(tkn *c, bool is_nmsc, bool unwinding);

// parser
node *parse(tkn *tkns, cmod mode);

bool isnumc(char chr);
bool ishexc(char chr);
bool validn(char chr);

bool matchs(char *str, bool(*func)(char));
bool hassym(char *str);
bool isscpd(char *str, u64 chr);
i16  iskeyw(char *str);

u64 upow(u64 b, u64 p);

u32  strtohex(char *str);
u64  strtoptr(char *str);
void free_str();

char *nodet_to_str(node *n);

// free the given node and all child nodes if `n->is_parent` is true
void free_node(node *n, bool silent);

// compilation error
void cmperr(imut char *err, tkn *arw, tkn *cmpl);
void wrning(imut char *wrn, tkn *arw, tkn *cmpl);

#define BIC

#include "bic.c"
#include "lexer.c"
#include "loadr.c"
#include "parser.c"