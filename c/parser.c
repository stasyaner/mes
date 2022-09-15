#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

/* char** lookahead; */

Node* parse() {
    printf("Parsed!\n");
    return program();
}

Node* program() {
    Node* result;
    result = malloc(sizeof(Node));
    result->type = PROGRAM;
    result->child = numericLiteral();

    return result;
}

Node* numericLiteral() {
    Node* result;
    result = malloc(sizeof(Node));
    result->type = NUMERIC_LITERAL;
    result->int_value = 777;
    /* const token = readTokenAndLookahead('Number'); */
    return result;
}
