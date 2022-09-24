#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "tokenizer.h"

#define LIST_SIZE 255

Token *lookahead_token = NULL;
Node *program();
Node *numeric_literal();
Node *string_literal();
Node *literal();
Node *expression();
Node *expression_statement();
Node *statement_list();
Node *statement();
Node *literal();
Node *relational_expression();
Node *binary_expression_wrapper();

void lookahead() {
    lookahead_token = get_next_token();
}

Token *read_token_and_lookahead(int acceptable_token_type) {
    Token *token = lookahead_token;

    assert_token_type(token, acceptable_token_type);

    lookahead();

    return token;
}

Node *parse() {
    lookahead();
    return program();
}

Node *program() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = program_node;
    result->child = statement_list();

    return result;
}

Node *statement_list() {
    Node *result;
    Node **list;
    int i = 0;

    list = malloc(sizeof(Node) * LIST_SIZE);
    list[i] = statement();

    for(i = 1; lookahead_token; i++) {
        list[i] = statement();
    }
    list[i + 1] = NULL;

    result = malloc(sizeof(Node));
    result->type = statement_list_node;
    result->children = list;

    return result;
}

Node *statement() {
    return expression_statement();
}

Node *expression_statement() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = expression_statement_node;
    result->child = expression();

    read_token_and_lookahead(semicolon_token); /* 'EOF */

    return result;
}

Node *expression() {
    switch(lookahead_token->type) {
        /* case 'JSXOpening':
        case 'JSXSelfClosing':
            return jsxExpression(); */
        default:
            return relational_expression();
    }
}

Node *relational_expression() {
    return binary_expression_wrapper(*literal, relational_token);
}

Node *binary_expression_wrapper(
    Node *(*left_expression)(),
    enum token_type operator_token
) {
    Node *left = left_expression();
    Node *right;
    Node *temp_left;

    while(lookahead_token && (lookahead_token->type == operator_token)) {
        Token *op_token = read_token_and_lookahead(operator_token);
        right = literal();
        temp_left = left;

        left->type = binary_expression_node;
        left->left = temp_left;
        left->right = right;
        left->operator = op_token->value;
    }

    return left;
}

Node *literal() {
    switch(lookahead_token->type) {
        case number_token:
            return numeric_literal();
        case string_token:
            return string_literal();
        default:
            exit(1);
    }
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
