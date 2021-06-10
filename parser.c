#include "9cc.h"

// whether string p start with q
bool startswith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// Consume a expected operator, and advance token list to next token
bool consume(char* op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || 
        memcmp(token->str, op, token->len))
        return false;

    token = token->next;
    return true;
}

// Expect a expected operator, and advance token list to next token
void expect(char* op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || 
        memcmp(token->str, op, token->len))
        error_at(token->str, "It's not '%c'.", op);
    token = token->next;
}

// Get a number from token, and advance token list to next token
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "There is not a number.");
    int val = token->val;
    token = token->next;
    return val;
}

// Check whether token list is at end.
bool at_eof() {
    return token->kind == TK_EOF;
}

// Create a new token
Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// Tokenize the statement string
Token* tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while(*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || 
            startswith(p, ">=") || startswith(p, "<=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p ,"Can't not tokenize.");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// expr = equality
Node* expr() {
    return equality();
}

// equality = relational ("==" relational | "!=" relational)*
Node* equality() {
    Node* node = relational();

    for(;;) {
        if (consume("=="))
            return new_binary(ND_EQ, node, relational());
        else if (consume("!="))
            return new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node* relational() {
    Node* node = add();

    for(;;) {
        if(consume("<"))
            return new_binary(ND_LT, node, add());
        else if(consume("<="))
            return new_binary(ND_LE, node, add());
        else if(consume(">"))
            return new_binary(ND_LT, add(), node);
        else if(consume(">="))
            return new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node* add() {
    Node* node = mul();

    for (;;) {
        if (consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul  = unary ("*" unary | "/" unary)*
Node* mul() {
    Node* node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
Node* unary() {
    if(consume("+")) {
        return unary();
    } else if(consume("-")) {
        return new_binary(ND_SUB, new_num(0), unary());
    } else
        return primary();
}

// primary = num | "(" expr ")"
Node *primary() {
    // consume ( and expect ) appear
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // expect get a number
    return new_num(expect_number());
}