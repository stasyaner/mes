#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "tokenizer.h"

#define LIST_SIZE 255
#define read_token_and_lookahead(tt)\
        read_token_and_lookahead_va(1, tt);
#define read_token_and_lookahead2(tt1, tt2)\
        read_token_and_lookahead_va(2, tt1, tt2);

static Token *read_token_and_lookahead_va(int n, ...);
static Node *file();
static Node *numeric_literal();
static Node *string_literal();
static Node *literal();
static Node *expression();
static Node *expression_statement();
static Node *statement_list();
static Node *statement();
static Node *literal();
static Node *relational_expression();
static Node *binary_expression_wrapper();
static Node *jsx_expression();
static Node *jsx_opening_element();
static Node *jsx_content();
static Node *jsx_closing_element();

static Token *lookahead_token = NULL;
static void lookahead() {
    lookahead_token = get_next_token();
}

Node *parse() {
    lookahead();
    return file();
}

static Node *file() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = file_node;
    result->child = statement_list();

    return result;
}

static Node *statement_list() {
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

static Node *statement() {
    return expression_statement();
}

static Node *expression_statement() {
    Node *result;
    result = malloc(sizeof(Node));
    result->type = expression_statement_node;
    result->child = expression();

    read_token_and_lookahead(semicolon_token); /* 'EOF */

    return result;
}

static Node *expression() {
    switch(lookahead_token->type) {
        case jsx_opening_token:
        case jsx_self_closing_token:
            return jsx_expression();
        default:
            return relational_expression();
    }
}

static Node *relational_expression() {
    return binary_expression_wrapper(*literal, relational_token);
}

static Node *binary_expression_wrapper(
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

static Node *jsx_expression() {
    Node *result;
    Node *opening_element = jsx_opening_element();
    Node *content = NULL;
    Node *closing_element = NULL;

    if (!opening_element->is_self_closing) {
        content = jsx_content();
        closing_element = jsx_closing_element();
    }

    result = malloc(sizeof(Node));
    result->type = jsx_expression_node;
    result->opening_element = opening_element;
    result->child = content;
    result->closing_element = closing_element;

    return result;
}

static Node *jsx_opening_element() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = jsx_opening_element_node;
    token = read_token_and_lookahead2(
        jsx_opening_token, jsx_self_closing_token);
    result->value = token->value;
    result->is_self_closing = (token->type == jsx_self_closing_token);

    return result;
}

static Node *jsx_content() {
    Node *result;
    Token *token = read_token_and_lookahead(jsx_text_token);

    result = malloc(sizeof(Node));
    result->type = jsx_content_node;
    result->value = token->value;

    return result;
}

static Node *jsx_closing_element() {
    Node *result;
    Token *token = read_token_and_lookahead(jsx_closing_token);

    result = malloc(sizeof(Node));
    result->type = jsx_closing_element_node;
    result->value = token->value;

    return result;
}

static Node *literal() {
    switch(lookahead_token->type) {
        case number_token:
            return numeric_literal();
        case string_token:
            return string_literal();
        default:
            exit(1);
    }
}

static Node *numeric_literal() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = numeric_literal_node;
    token = read_token_and_lookahead(number_token);
    result->value = token->value;

    return result;
}

static Node *string_literal() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = string_literal_node;
    token = read_token_and_lookahead(string_token);
    result->value = token->value;

    return result;
}

static Token *read_token_and_lookahead_va(int n, ...) {
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
