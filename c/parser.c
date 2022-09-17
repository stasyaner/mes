#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "tokenizer.h"

Token *lookahead_token = NULL;
Node *program();
Node *numeric_literal();
Node *string_literal();

Token *read_token_and_lookahead(int acceptable_token_type) {
    Token *token = lookahead_token;

    assert_token_type(token, acceptable_token_type);

    lookahead_token = get_next_token();

    return token;
}

Node *parse() {
    lookahead_token = get_next_token();
    return program();
}

Node *program() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = program_node;
    result->child = string_literal();

    return result;
}

Node *numeric_literal() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = numeric_literal_node;
    token = read_token_and_lookahead(number_token);
    result->int_value = atoi(token->value);

    return result;
}

Node *string_literal() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = string_literal_node;
    token = read_token_and_lookahead(string_token);
    result->str_value = token->value;

    return result;
}
