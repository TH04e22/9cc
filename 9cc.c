#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* Typedef */
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
	char * str;
};

// Abstract syntax tree node type
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;

// Abstract syntax tree node structure
struct Node {
	NodeKind kind; // node type
	Node *lhs; // left
	Node *rhs; // right
	int val; // Only using in kind is ND_NUM
};

// Current processsing token
Token *token;
char *user_input;

// Process error function
// like printf argument
void error( char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	vfprintf( stderr, fmt, ap );
	fprintf( stderr, "\n" );
	exit(1);
}

// report error position
void error_at( char *loc, char *fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );

	int pos = loc - user_input;
	fprintf( stderr, "%s\n", user_input );
	fprintf( stderr, "%*s", pos, "" );
	fprintf( stderr, "^ " );
	vfprintf( stderr, fmt, ap );
	fprintf( stderr, "\n" );
	exit(1);
}

// If next token is expected, read next token and continue
bool consume( char op ) {
	if( token->kind != TK_RESERVED || token->str[0] != op )
		return false;
	token = token->next;
	return true;
}

// Next token is expected, then read next token and go on,
// or show error
void expect( char op ) {
	if ( token->kind != TK_RESERVED || token->str[0] != op )
		error("Not '%c'", op );
	token = token->next;
}

// If next token is number, read next token and go on, return the
// number; or show error 
int expect_number() {
	if( token->kind != TK_NUM )
		error_at( token->str ,"Not number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

                   /* Tokenize */
// Create a new token, and linked by current token list
Token *new_token( TokenKind kind, Token *cur, char *str ) {
	Token *tok = calloc(1,sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// Tokenize string p and return token list
Token *tokenize( char *p ) {
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		// ignore white space
		if (isspace(*p)) {
			p++;
			continue;
		}

		if ( *p == '+' || *p == '-' || *p == '(' || *p == ')' || *p == '*' || *p == '/' ) {
			cur = new_token( TK_RESERVED, cur, p++ );
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token( TK_NUM, cur ,p );
			cur->val = strtol( p, &p, 10 );
			continue;
		}

		error("Cannot tokenize");
	}

	new_token( TK_EOF, cur, p );
	return head.next;
}

/* Parser */
/*
    expr  = mul ("+" mul | "-" mul)*
	mul   = unary ("*" unary | "/" unary)*
	unary = ("+" | "-")? term
	term  = num | "(" expr ")" 
*/
Node *new_node( NodeKind kind, Node *lhs, Node *rhs );
Node *new_node_num( int val );
Node *term();
Node *unary();
Node *mul();
Node *expr();

/* Code Generation */
void gen( Node *node ) {
	if ( node->kind == ND_NUM ) {
		printf( "	push %d\n", node->val );
		return;
	}

	gen( node->lhs );
	gen( node->rhs );

	printf( "	pop rdi\n" );
	printf( "	pop rax\n" );

	switch( node->kind ) {
		case ND_ADD:
			printf( "	add rax, rdi\n" );
			break;
		case ND_SUB:
			printf( "	sub rax, rdi\n" );
			break;
		case ND_MUL:
			printf( "	imul rax, rdi\n" );
			break;
		case ND_DIV:
			printf( "	cqo\n" );
			printf( "	idiv rdi\n");
			break;

	}

	printf( "	push rax\n" );
}

int main( int argc, char **argv) {
    if( argc != 2 ) {
    	fprintf( stderr, "Arguments  error\n" );
        return 1;
	}
	
	// Tokenize and parser
	user_input = argv[1];
	token = tokenize(argv[1]);
	Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    
	// Traverse grammer tree and generate code
	gen( node );

	printf("	pop rax\n");	
    printf("	ret\n");
    return 0;
}

/* Parser */
Node *new_node( NodeKind kind, Node *lhs, Node *rhs ) {
	Node *node = calloc( 1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num( int val ) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

// term = num | "(" expr ")"
Node *term() {
	// If next token is "(", it should be "(" expr ")".
	if( consume('(') ) {
		Node *node = expr();
		expect(')');
		return node;
	}

	// Otherwise, it should be number
	return new_node_num(expect_number());
}

// mul = unary ( "*" unary | "/" unary )*
Node *mul() {
	Node *node = unary();

	for(;;) {
		if( consume('*') )
			node = new_node(ND_MUL, node, unary());
		else if( consume('/') )
			node = new_node(ND_DIV,node,unary());
		else
			return node;
	}
}

// expr = mul ("+" mul | "-" mul ) *
Node *expr() {
	Node *node = mul();
	
	for(;;) {
		if( consume('+') )
			node = new_node( ND_ADD, node, mul() );
		else if( consume('-') )
			node = new_node( ND_SUB, node, mul() );
		else
			return node;
	}
}

// unary = ("+" | "-")? term
Node *unary() {
  if (consume('+'))
    return term();
  if (consume('-'))
    return new_node(ND_SUB, new_node_num(0), term());
  return term();
}