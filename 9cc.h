#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* token.c */
typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

// Token Linking list structure
struct Token { 
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

extern Token *token;
extern char *user_input;
bool consume( char *op );
void expect( char *op );
int expect_number();
bool at_eof();
Token *new_token( TokenKind kind, Token *cur, char *str, int len );
Token *tokenize( char *p );

/* parse.c */
// Abstract syntax tree node type
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // Integer
	ND_EQ,  // ==
  	ND_NE,  // !=
  	ND_LE,  // <=
	ND_L,	// <
} NodeKind;

typedef struct Node Node;

// Abstract syntax tree node structure
struct Node {
	NodeKind kind; // node type
	Node *lhs; // left
	Node *rhs; // right
	int val; // Only using in kind is ND_NUM
};

Node *new_node( NodeKind kind, Node *lhs, Node *rhs );
Node *new_node_num( int val );
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

/* util.c */
void error( char* fmt, ... );
void error_at( char *loc, char *fmt, ... );

/* codegen.c */
void gen( Node *node );