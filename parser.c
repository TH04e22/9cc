#include "9cc.h"

char* user_input = NULL;
Token* token = NULL;
Node* code[100];
LVar* locals = NULL;

// whether string p start with q
bool startswith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// the character is alphabet or number or '_'
int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            (c == '_');
}

// Consume a expected operator, and advance token list to next token
bool consume(char* op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || 
        memcmp(token->str, op, token->len))
        return false;

    token = token->next;
    return true;
}

// Consume by token type
bool consume_type(TokenKind kind) {
    if(token->kind != kind)
        return false;

    token = token->next;
    return true;
}

// Consume a identifier
Token* consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;

    Token* cur = token;
    token = token->next;
    return cur;
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
        if (isblank(*p) || *p == '\n' || *p == '\r') {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || 
            startswith(p, ">=") || startswith(p, "<=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (isalpha(*p)) {
            // start of variable
            char* q = p;
            do {
                p++;
            } while(is_alnum(*p));
            cur = new_token(TK_IDENT, cur, q, p-q);
            continue;
        }

        if (strchr("+-*/()<>=;", *p)) {
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

LVar* find_LVar(Token* tok) {
    for(LVar* var = locals; var != NULL; var = var->next) {
        if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

// stmt = expr ";"| "return" expr ";"
Node* stmt() {
    Node *node;
    if(consume_type(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }

    expect(";");
    return node;
}

// expr = assign
Node* expr() {
    return assign();
}

// assign = equality (=assign)?
Node* assign() {
    Node* node = equality();

    for(;;) {
        if(consume("=")) {
            node = new_binary(ND_ASSIGN, node, assign());
        }
        
        return node;
    }
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

// primary = num | ident |"(" expr ")"
Node *primary() {
    // consume ( and expect ) appear
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token* tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar* lvar = find_LVar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));

            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;

            if (locals != NULL) {
                lvar->offset = locals->offset + 8;
            } else {
                lvar->offset = 8;
            }
        
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    // expect get a number
    return new_num(expect_number());
}