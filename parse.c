#include "9cc.h"
/*
    expr       = equality
	equality   = relational ("==" relational | "!=" relational)*
	relational = add ("<" add | "<=" add | ">" add | ">=" add)*
	add        = mul ("+" mul | "-" mul)*
	mul        = unary ("*" unary | "/" unary)*
	unary      = ("+" | "-")? term
	term       = num | "(" expr ")"
*/

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

// expr = equality
Node *expr() {
	Node *node = equality();
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

// unary = ("+" | "-")? term
Node *unary() {
  	if (consume("+"))
    	return term();
  	if (consume("-"))
    	return new_node(ND_SUB, new_node_num(0), term());
  	return term();
}

// term = num | "(" expr ")"
Node *term() {
	// If next token is "(", it should be "(" expr ")".
	if( consume("(") ) {
		Node *node = expr();
		expect(")");
		return node;
	}

	// Otherwise, it should be number
	return new_node_num(expect_number());
}