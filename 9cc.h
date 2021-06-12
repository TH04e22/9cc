#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* user_input;

// utils.c
// print error message with variable argument list
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);

// parser.c
typedef enum {
    TK_RESERVED, // reserved token
    TK_RETURN,   // return
    TK_IDENT,    // identifier
    TK_NUM,      // number
    TK_EOF,      // end of token list
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;    // stored num when kind is TK_NUM
    char *str;
    int len;    // operator character length
};

extern Token* token;

Token* tokenize(char* p);
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
bool consume(char* op);
void expect(char* op);
int expect_number();
bool at_eof();

// Abstract tree node type
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE, // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_ASSIGN, // =
    ND_LVAR,
    ND_RETURN, // return
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val; // use for when node kind is ND_NUM
    int offset; // use for when node kind is ND_LVAR
};

extern Node* code[100];

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

typedef struct LVar LVar;

struct LVar {
    LVar* next; // next variable
    char* name; // variable name
    int len;    // name length
    int offset; // offset from RBP
};

extern LVar* locals;
LVar* find_LVar(Token* tok);

/*
program    = stmt*
stmt       = expr ";"| "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
*/

void program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

// codegen.c
void gen(Node* node);