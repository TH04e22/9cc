#include "9cc.h"

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

