#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* token.c */
typedef enum {
	TK_RESERVED, // 符號
	TK_NUM,      // 數值
	TK_EOF,      // 結束的標記
	TK_IDENT,    // 識別符號
	TK_RETURN,   // 回傳return
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
bool consume_token( TokenKind kind );
Token *consume_ident();
void expect( char *op );
int expect_number();
bool at_eof();
Token *new_token( TokenKind kind, Token *cur, char *str );
void tokenize();

typedef struct LVar LVar;

// Local variable data tructure
struct LVar {
	LVar *next; // Next variable or NULL
	char *name; // Variable Name
	int len;    // Name length
	int offset; // offset start from RBP
};

extern LVar *locals;
LVar *find_LVar( Token *tok ); // Find variable in the token list

/* parse.c */
// Abstract syntax tree node type
typedef enum {
	ND_ADD,    // +
	ND_SUB,    // -
	ND_MUL,    // *
	ND_DIV,    // /
	ND_EQ,     // ==
  	ND_NE,     // !=
  	ND_LE,     // <=
	ND_L,	   // <
	ND_ASSIGN, // =
	ND_LVAR,   // Local Variable
	ND_NUM,    // Integer
	ND_RETURN, // return
} NodeKind;

typedef struct Node Node;

// Abstract syntax tree node structure
struct Node {
	NodeKind kind; // node type
	Node *lhs;     // left
	Node *rhs;     // right
	int val;       // Only using in kind is ND_NUM
	int offset;    // Only using in kind is ND_LVAR
};

Node *new_node( NodeKind kind, Node *lhs, Node *rhs );
Node *new_node_num( int val );
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

/* util.c */
void error( char* fmt, ... );
void error_at( char *loc, char *fmt, ... );

/* codegen.c */
void gen_lval(Node *node);
void gen( Node *node );