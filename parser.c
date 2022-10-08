#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "tokenizer.h"

#define LIST_SIZE 255
#define JSX_CONTENT_LENGTH 255
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
/* static Node *binary_expression_wrapper(); */
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
    for(i = 0; lookahead_token && i < LIST_SIZE; i++) {
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
        case number_token:
        case string_token:
            return relational_expression();
        default:
            return jsx_expression();
    }
}

static Node *relational_expression() {
    Node *left = literal();
    Node *right;
    Node *temp_left;

    while(
        lookahead_token && (
            (lookahead_token->type == opening_angle_token) ||
            (lookahead_token->type == closing_angle_token)
    )) {
        char *operator = lookahead_token->value;

        lookahead();
        right = literal();
        temp_left = malloc(sizeof(Node));
        memcpy(temp_left, left, sizeof(Node));

        left->type = binary_expression_node;
        left->left = temp_left;
        left->right = right;
        left->operator = operator;
    }

    return left;
}

/* static Node *binary_expression_wrapper(
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
} */

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
    char is_self_closing = 0;
    Node *result;
    Token *identifier = NULL;

    read_token_and_lookahead(opening_angle_token);
    identifier = read_token_and_lookahead(identifier_token);
    if(lookahead_token->type == slash_token) {
        is_self_closing = 1;
        lookahead();
    }
    read_token_and_lookahead(closing_angle_token);

    result = malloc(sizeof(Node));
    result->type = jsx_opening_element_node;
    result->value = identifier->value;
    result->is_self_closing = is_self_closing;

    return result;
}

static Node *jsx_content() {
    Node *result;
    char *p;

    result = malloc(sizeof(Node));
    result->type = jsx_content_node;
    result->value = malloc(JSX_CONTENT_LENGTH);
    p = result->value;

    while(lookahead_token->type != opening_angle_token) {
        int l = strlen(lookahead_token->value);
        int l_total = result->value - p;

        if(l_total >= (JSX_CONTENT_LENGTH - 1)) {
            fprintf(
                stderr,
                "JSX text is too long. Max length is %d.\n",
                JSX_CONTENT_LENGTH
            );
            exit(1);
        }

        strncpy(p, lookahead_token->value, l);
        *(p + l) = ' ';
        p += l + 1;
        free(lookahead_token->value);
        free(lookahead_token);
        lookahead();
    }
    *(p - 1) = '\0';

    return result;
}

static Node *jsx_closing_element() {
    Node *result;
    Token *identifier;

    read_token_and_lookahead(opening_angle_token);
    read_token_and_lookahead(slash_token);
    identifier = read_token_and_lookahead(identifier_token);
    read_token_and_lookahead(closing_angle_token);

    result = malloc(sizeof(Node));
    result->type = jsx_closing_element_node;
    result->value = identifier->value;

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
