#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
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
Node *jsx_expression();
Node *jsx_opening_element();
Node *jsx_content();
Node *jsx_closing_element();

#define read_token_and_lookahead(tt)\
        read_token_and_lookahead_va(1, tt);
#define read_token_and_lookahead2(tt1, tt2)\
        read_token_and_lookahead_va(2, tt1, tt2);

void lookahead() {
    lookahead_token = get_next_token();
}

Token *read_token_and_lookahead_va(int n, ...) {
    va_list vl;
    enum token_type tt;
    int i;
    char check_result;
    Token *token = lookahead_token;

    va_start(vl, n);
    for(i = 0; i < n; i++) {
        tt = va_arg(vl, enum token_type);
        check_result = check_token_type(token, tt);
        if(!check_result) {
            break;
        }
    }
    va_end(vl);

    if(check_result) {
        fprintf(stderr, "Unexpected token.\n");
        exit(1);
    }

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
        case jsx_opening_token:
        case jsx_self_closing_token:
            return jsx_expression();
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
        temp_left = malloc(sizeof(Node));
        memcpy(temp_left, left, sizeof(Node));

        left->type = binary_expression_node;
        left->left = temp_left;
        left->right = right;
        left->operator = op_token->value;
    }

    return left;
}

Node *jsx_expression() {
    Node *result;
    Node *opening_element = jsx_opening_element();
    Node *content = NULL;
    Node *closing_element = NULL;

    /* if (!opening_element->is_self_closing) {
        content = jsx_content();
        closing_element = jsx_closing_element();
    } */

    result = malloc(sizeof(Node));
    result->type = jsx_expression_node;
    result->opening_element = opening_element;
    result->child = content;
    result->closing_element = closing_element;

    return result;
}

Node *jsx_opening_element() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = jsx_opening_element_node;
    token = read_token_and_lookahead2(
        jsx_opening_token, jsx_self_closing_token);
    result->str_value = token->value;
    result->is_self_closing = (token->type == jsx_self_closing_token);

    return result;
}

/* Node *jsx_content() {
    Node *result;
    Token *token = read_token_and_lookahead(jsx_text_token);

    result = malloc(sizeof(Node));
    result->type = jsx_content_node;
    result->str_value = token->value;

    return result;
}

Node *jsx_closing_element() {
    Node *result;
    Token *token = read_token_and_lookahead(jsx_closing_token);

    result = malloc(sizeof(Node));
    result->type = jsx_closing_element_node;
    result->str_value = token->value;

    return result;
} */

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
