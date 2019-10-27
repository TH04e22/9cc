#include "9cc.h"

// Current processsing token
Token *token;
char *user_input;

// If next token is expected, read next token and continue
bool consume( char *op ) {
  	if ( token->kind != TK_RESERVED ||
     	strlen(op) != token->len ||
      	memcmp(token->str, op, token->len))
    	return false;
  	token = token->next;
  	return true;
}

// Next token is expected, then read next token and go on,
// or show error
void expect( char *op ) {
	if ( token->kind != TK_RESERVED ||
	 	strlen(op) != token->len ||
		memcmp(token->str, op, token->len ) )
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
Token *new_token( TokenKind kind, Token *cur, char *str, int len ) {
	Token *tok = calloc(1,sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
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

		if( *p == '<' || *p == '!' || *p == '=' || *p == '>' ) {
			if( *(p + 1) == '=') {
				cur = new_token( TK_RESERVED, cur, p, 2);
				p += 2;
			} else {
				cur = new_token( TK_RESERVED, cur, p++, 1 );
			}
			continue;
		}

		if ( *p == '+' || *p == '-' || *p == '(' || *p == ')' || *p == '*' || *p == '/' ) {
			cur = new_token( TK_RESERVED, cur, p++, 1 );
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token( TK_NUM, cur ,p , 0 );
			cur->val = strtol( p, &p, 10 );
			continue;
		}

		error("Cannot tokenize");
	}

	new_token( TK_EOF, cur, p, 1 );
	return head.next;
}