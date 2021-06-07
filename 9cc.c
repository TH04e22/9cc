#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};

Token* token;

// print error message with variable argument list
void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// Consume a expected operator, and advance token list to next token
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// Expect a expected operator, and advance token list to next token
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("It's not '%c'.", op);
    token = token->next;
}

// Get a number from token, and advance token list to next token
int expect_number() {
    if (token->kind != TK_NUM)
        error("There is not a number.");
    int val = token->val;
    token = token->next;
    return val;
}

// Check whether token list is at end.
bool at_eof() {
    return token->kind == TK_EOF;
}

// Create a new token
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
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

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Can't not tokenize.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char** argv) {
    if(argc != 2) {
        error("Parameter count %d is incorrect\n", argc);
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while(!at_eof()) {
        if(consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    printf("    ret\n");
    return 0;
}