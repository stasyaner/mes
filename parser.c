#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "tokenizer.h"

#define MAX_LIST_SIZE 255
#define JSX_MAX_TEXT_LENGTH 255
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
static Node *identifier();
static Node *literal();
static Node *primary_expression();
static Node *relational_expression();
/* static Node *binary_expression_wrapper(); */
static Node *jsx_element(char is_nested);
static Node *jsx_opening_element(char is_nested);
static Node *jsx_attribute();
static Node *jsx_expression();
static Node *jsx_text();
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
    int i;

    list = malloc(sizeof(Node) * MAX_LIST_SIZE);
    for(i = 0; lookahead_token; i++) {
        if(i == (MAX_LIST_SIZE - 1)) {
            fprintf(stderr, "Too many statements. Max is %d.\n", MAX_LIST_SIZE);
            exit(1);
        }
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
        case identifier_token:
            return relational_expression();
        default:
        case opening_angle_token:
            return jsx_element(0);
    }
}

static Node *relational_expression() {
    Node *left = primary_expression();
    Node *right;
    Node *temp_left;

    while(
        lookahead_token && (
            (lookahead_token->type == opening_angle_token) ||
            (lookahead_token->type == closing_angle_token)
    )) {
        char *operator = lookahead_token->value;

        lookahead();
        right = primary_expression();
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

static Node *jsx_element(char is_nested) {
    Node *result;
    Node *opening_element = jsx_opening_element(is_nested);
    Node **children = NULL;
    Node *closing_element = NULL;
    int i;

    if (!opening_element->is_self_closing) {
        for(i = 0;; i++) {
            if(i == (MAX_LIST_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Too many jsx children. Max is %d.\n",
                    MAX_LIST_SIZE
                );
                exit(1);
            }
            if(lookahead_token->type == opening_angle_token) {
                lookahead();
                if(lookahead_token->type == slash_token) {
                    break;
                } else {
                    if(!children) {
                        children = malloc(sizeof(Node) * MAX_LIST_SIZE);
                    }
                    children[i] = jsx_element(1);
                }
            } else if(lookahead_token->type == opening_curly_token) {
                if(!children) {
                    children = malloc(sizeof(Node) * MAX_LIST_SIZE);
                }
                children[i] = jsx_expression();
            } else {
                if(!children) {
                    children = malloc(sizeof(Node) * MAX_LIST_SIZE);
                }
                children[i] = jsx_text();
            }
        }
        if(children) {
            children[i + 1] = NULL;
        }
        closing_element = jsx_closing_element();
    }

    result = malloc(sizeof(Node));
    result->type = jsx_element_node;
    result->opening_element = opening_element;
    result->children = children;
    result->closing_element = closing_element;

    return result;
}

static Node *jsx_opening_element(char is_nested) {
    char is_self_closing = 0;
    Node *result;
    Node *id = NULL;
    Node **attributes = NULL;
    int i;

    if(!is_nested) {
        read_token_and_lookahead(opening_angle_token);
    }
    if(lookahead_token->type == identifier_token) {
        id = identifier();
    }
    if(lookahead_token->type == identifier_token) {
        attributes = malloc(sizeof(Node) * MAX_LIST_SIZE);
        for(i = 0; lookahead_token->type == identifier_token; i++) {
            if(i == (MAX_LIST_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Too many jsx children. Max is %d.\n",
                    MAX_LIST_SIZE
                );
                exit(1);
            }
            attributes[i] = jsx_attribute();
        }
        attributes[i + 1] = NULL;
    } else if(lookahead_token->type == slash_token) {
        is_self_closing = 1;
        lookahead();
    }
    read_token_and_lookahead(closing_angle_token);

    result = malloc(sizeof(Node));
    result->type = jsx_opening_element_node;
    result->child = id;
    result->children = attributes;
    result->is_self_closing = is_self_closing;

    return result;
}

static Node *jsx_attribute() {
    Node *result;
    Node *id = identifier();
    Node *value;

    read_token_and_lookahead(equality_token);
    if(lookahead_token->type == opening_curly_token) {
        value = jsx_expression();
    } else {
        value = string_literal();
    }

    result = malloc(sizeof(Node));
    result->type = jsx_attribute_node;
    result->left = id;
    result->right = value;

    return result;
}

static Node *jsx_expression() {
    Node *result;
    Node *content;

    read_token_and_lookahead(opening_curly_token);
    content = primary_expression();
    read_token_and_lookahead(closing_curly_token);

    result = malloc(sizeof(Node));
    result->type = jsx_expression_node;
    result->child = content;

    return result;
}

static Node *jsx_text() {
    Node *result;
    char *p;

    result = malloc(sizeof(Node));
    result->type = jsx_text_node;
    result->value = malloc(JSX_MAX_TEXT_LENGTH);
    p = result->value;

    while(lookahead_token->type != opening_angle_token) {
        int l = strlen(lookahead_token->value);
        int l_total = result->value - p;

        if(l_total >= (JSX_MAX_TEXT_LENGTH - 1)) {
            fprintf(
                stderr,
                "JSX text is too long. Max length is %d.\n",
                JSX_MAX_TEXT_LENGTH
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
    Node *id = NULL;

    read_token_and_lookahead(slash_token);
    if(lookahead_token->type == identifier_token) {
        id = identifier();
    }
    read_token_and_lookahead(closing_angle_token);

    result = malloc(sizeof(Node));
    result->type = jsx_closing_element_node;
    result->child = id;

    return result;
}

static Node *primary_expression() {
    switch(lookahead_token->type) {
        case number_token:
        case string_token:
            return literal();
        default:
        case identifier_token:
            return identifier();
    }
}

static Node *identifier() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = identifier_node;
    token = read_token_and_lookahead(identifier_token);
    result->value = token->value;

    return result;
}

static Node *literal() {
    switch(lookahead_token->type) {
        case number_token:
            return numeric_literal();
        default:
        case string_token:
            return string_literal();
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
