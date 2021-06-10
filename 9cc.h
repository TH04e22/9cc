#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* user_input;

// utils.c
// print error message with variable argument list
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);

// parser.c
typedef enum {
    TK_RESERVED, // reserved token
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

Token* token;

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
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val; // use for when node kind is ND_NUM
};

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

/*
expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | "(" expr ")"
*/

Node* expr();
Node *equality();
Node *relational();
Node *add();
Node* mul();
Node* unary();
Node* primary();

// codegen.c
void gen(Node* node);