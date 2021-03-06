#include "9cc.h"

/*
	program    = stmt*
	stmt       = expr ";" | "return" expr ";"
	expr       = assign
	assign     = equality ("=" assign)?
	equality   = relational ("==" relational | "!=" relational)*
	relational = add ("<" add | "<=" add | ">" add | ">=" add)*
	add        = mul ("+" mul | "-" mul)*
	mul        = unary ("*" unary | "/" unary)*
	unary      = ("+" | "-")? primary
	primary    = num | ident | "(" expr ")"
*/ 

Node *code[100];

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


// program    = stmt*
void program() {
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

// stmt = expr ";" | "return" expr ";"
Node *stmt() {
	Node *node;
	if( consume_token( TK_RETURN ) ) {
		node = calloc( 1, sizeof( Node ) );
		node->kind = ND_RETURN;
		node->lhs = expr();
	} else {
		node = expr();
	}

	if (!consume(";"))
    	error_at( token->str , "is not ; token!");
	return node;
}

// expr = assign
Node *expr() {
	return assign();
}

// 	assign  = equality ("=" assign )?
Node *assign() {
	Node *node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return node;
}

// equality   = relational ("==" relational | "!=" relational)*
Node *equality() {
	Node *node = relational();
	for(;;) {
		if( consume("==") )
			node = new_node(ND_EQ, node, relational());
		else if( consume("!=") )
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
	Node *node = add();
	for(;;) {
		if( consume("<") )
			node = new_node( ND_L, node, add() );
		else if( consume("<=") )
			node = new_node( ND_LE, node, add() );
		else if( consume(">") )
			node = new_node( ND_L, add(), node );
		else if( consume(">="))
			node = new_node( ND_LE, add(), node );
		else
			return node;
	}
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
	Node *node = mul();
	for(;;) {
		if( consume("+") )
			node = new_node( ND_ADD, node, mul() );
		else if( consume("-") )
			node = new_node( ND_SUB, node, mul() );
		else
			return node;
	}
}

// mul = unary ( "*" unary | "/" unary )*
Node *mul() {
	Node *node = unary();

	for(;;) {
		if( consume("*") )
			node = new_node(ND_MUL, node, unary());
		else if( consume("/") )
			node = new_node(ND_DIV,node,unary());
		else
			return node;
	}
}

// unary = ("+" | "-")? primary
Node *unary() {
  	if (consume("+"))
    	return primary();
  	if (consume("-"))
    	return new_node(ND_SUB, new_node_num(0), primary());
  	return primary();
}

// primary = num | ident | "(" expr ")"
Node *primary() {
	// If next token is "(", it should be "(" expr ")".
	if( consume("(") ) {
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident();
  	if (tok) {
    	Node *node = calloc(1, sizeof(Node));
    	node->kind = ND_LVAR;
    	
		LVar *lvar = find_LVar( tok );
		if( lvar ) {
			node->offset = lvar->offset;
		} else {
			lvar = calloc( 1, sizeof( LVar ) );
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			if( locals )
				lvar->offset = locals->offset + 8;
			else
				lvar->offset = 0;
			node->offset = lvar->offset;
			locals = lvar;
		}
    	return node;
  	}

	// Otherwise, it should be number
	return new_node_num(expect_number());
}