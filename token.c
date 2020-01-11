#include "9cc.h"

int is_alnum( char c ) {
	return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

int is_alpha( char c ) {
	return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           (c == '_');
}

int is_digit( char c ) {
	return ('0' <= c && c <= '9');
}

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

// If next token type is expected, read next token and continue
bool consume_token( TokenKind kind ) {
	if( token->kind != kind )
		return false;
	token = token->next;
	return true;
}

// If next token is identifier then return token, else return NULL
Token *consume_ident() {
	if( token->kind != TK_IDENT )
		return NULL;
	Token *temp = token;
	token = token->next;
	return temp;
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
Token *new_token( TokenKind kind, Token *cur, char *str ) {
	Token *tok = calloc(1,sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// Tokenize string p and return token list
void tokenize() {
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;
	
	while (*p) {
		// ignore white space
		if (isspace(*p)) {
			p++;
			continue;
		}

		// return keyword
		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
			cur = new_token( TK_RETURN, cur, p );
			cur->kind = TK_RETURN;
			cur->str = p;
			p += 6;
			continue;
		}

		// identifier
		if ( is_alpha( *p ) ) {
			cur = new_token( TK_IDENT, cur, p );
			cur->len = 1;
			while( is_alnum( *(++p) ) ) {
				cur->len++;
			}
			continue;
		} else if( is_digit(*p) ) {
			char* temp = p;
			while( is_digit(*(++temp)) ) {};
			
			if( is_alpha( *temp ) )
				error_at( temp-1, "Identifier can't begin with digit!");
		}

		if( *p == '<' || *p == '!' || *p == '=' || *p == '>' ) {
			if( *(p + 1) == '=') {
				cur = new_token( TK_RESERVED, cur, p );
				p += 2;
				cur->len = 2;
			} else {
				cur = new_token( TK_RESERVED, cur, p++ );
				cur->len = 1;
			}
			continue;
		}

		if ( *p == '+' || *p == '-' || *p == '(' || *p == ')' || *p == '*' || *p == '/'  || *p == ';' ) {
			cur = new_token( TK_RESERVED, cur, p++ );
			cur->len = 1;
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token( TK_NUM, cur ,p );
			cur->val = strtol( p, &p, 10 );
			cur->len = 0;
			continue;
		}

		error("Cannot tokenize");
	}

	new_token( TK_EOF, cur, p );
	token = head.next;
}

/* Variable */
LVar *locals = NULL;

LVar *find_LVar( Token *tok ) {
	for ( LVar *var = locals; var; var = var->next ) 
		if( var->len == tok->len && !memcmp( tok->str, var->name, tok->len ))
			return var;
	return NULL;
}