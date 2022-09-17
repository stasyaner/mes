#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "tokenizer.h"

Token *lookahead_token = NULL;

char *read_token_and_lookahead(int acceptable_token_type) {
    Token *token = lookahead_token;

    assert_token_type(token, acceptable_token_type);

    lookahead_token = get_next_token();

    return token->value;
}

Node *parse() {
    lookahead_token = get_next_token();
    return program();
}

Node *program() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = program_node;
    result->child = numeric_literal();

    return result;
}

Node *numeric_literal() {
    Node *result;
    char *token;

    result = malloc(sizeof(Node));
    result->type = numeric_literal_node;
    token = read_token_and_lookahead(number_token);
    result->int_value = atoi(token);

    return result;
}
