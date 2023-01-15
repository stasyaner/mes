#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "tokenizer.h"

#define MAX_LIST_SIZE 255
#define JSX_MAX_TEXT_LENGTH 255
#define read_token_and_lookahead(tt)\
        read_token_and_lookahead_va(1, tt)
#define read_token_and_lookahead2(tt1, tt2)\
        read_token_and_lookahead_va(2, tt1, tt2)

union jsx_child_function {
    Node *(*jsx_element_nested)(long start_position);
    Node *(*other_one_argument_function)(void);
};

static Token *read_token_and_lookahead_va(int n, ...);
static Node *file();
static Node *string_literal();
static Node *statement_list();
static Node *identifier();
static Node *jsx_element_base(char is_nested, long start_position);
static Node *jsx_element(void);
static Node *jsx_element_nested();
static Node *jsx_opening_element(char is_nested, long start_position);
static Node *jsx_attribute();
static Node *jsx_expression();
static Node *jsx_text();
static Node *jsx_closing_element();

static Token *lookahead_token = NULL;
static void lookahead() {
    lookahead_token = get_next_token();
}
static void lookahead_w_space_parsing() {
    lookahead_token = get_next_token_w_space();
}

Node *parse(char *input) {
    tokenizer_init(input);
    lookahead_w_space_parsing();
    return file();
}

static Node *file() {
    Node *result;
    Node *child = statement_list();

    result = malloc(sizeof(Node));
    result->type = file_node;
    result->child = child;
    result->start = child->start;
    result->end = child->end;

    return result;
}

static Node *statement_list() {
    Node *result, *st;
    Node **list = NULL;
    int i;

    for(i = 0;; i++) {
        if(i == (MAX_LIST_SIZE - 1)) {
            fprintf(stderr, "Too many statements. Max is %d.\n", MAX_LIST_SIZE);
            exit(1);
        }
        if(!lookahead_token) {
            break;
        }
        if(lookahead_token->type != opening_angle_token) {
            i--;
            free(lookahead_token->value);
            free(lookahead_token);
            lookahead();
            continue;
        }
        st = jsx_element();
        if(!st) {
            i--;
            /* TODO: free jsx_element */
            lookahead();
            continue;
        }
        if(!list) {
            list = malloc(sizeof(Node) * MAX_LIST_SIZE);
        }
        list[i] = st;
    }
    if(list) {
        list[i] = NULL;
    }

    result = malloc(sizeof(Node));
    result->type = statement_list_node;
    result->children = list;
    result->start = list[0]->start;
    result->end = list[i - 1]->end;

    return result;
}

static Node *jsx_element_base(char is_nested, long start_position) {
    Node *result;
    Node *opening_element = jsx_opening_element(is_nested, start_position);
    Node **children = NULL;
    Node *closing_element = NULL;
    Node *child;
    union jsx_child_function child_func;
    long nested_start_position = -1;
    int i;

    if(!opening_element) {
        return NULL;
    }

    if(!opening_element->is_self_closing) {
        for(i = 0;; i++) {
            if(i == (MAX_LIST_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Too many jsx children. Max is %d.\n",
                    MAX_LIST_SIZE
                );
                exit(1);
            }

            if(!lookahead_token) {
                return NULL;
            }

            if(lookahead_token->type == opening_angle_token) {
                nested_start_position = lookahead_token->start;
                lookahead();
                if(lookahead_token->type == slash_token) {
                    break;
                } else {
                    child_func.jsx_element_nested = jsx_element_nested;
                }
            } else if(lookahead_token->type == opening_curly_token) {
                child_func.other_one_argument_function = jsx_expression;
            } else {
                child_func.other_one_argument_function = jsx_text;
            }

            if(nested_start_position > 0) {
                child = child_func.jsx_element_nested(nested_start_position);
            } else {
                child = child_func.other_one_argument_function();
            }

            if(!child) {
                return NULL;
            }
            if(!children) {
                children = malloc(sizeof(Node) * MAX_LIST_SIZE);
            }
            children[i] = child;
        }
        if(children) {
            children[i] = NULL;
        }
        closing_element = jsx_closing_element();
    }

    result = malloc(sizeof(Node));
    result->type = jsx_element_node;
    result->opening_element = opening_element;
    result->children = children;
    result->closing_element = closing_element;
    result->start = is_nested ? start_position : opening_element->start;
    result->end = opening_element->is_self_closing ?
        opening_element->end
        : closing_element->end;

    return result;
}

static Node *jsx_element(void) {
    return jsx_element_base(0, -1);
}

static Node *jsx_element_nested(long start_position) {
    return jsx_element_base(1, start_position);
}

static Node *jsx_opening_element(char is_nested, long start_position) {
    char is_self_closing = 0;
    Node *result;
    Node *id = NULL;
    Node **attributes = NULL;
    Token *start_token = NULL;
    Token *end_token = NULL;
    int i;

    if(!is_nested) {
        start_token = read_token_and_lookahead(opening_angle_token);
    }
    if(lookahead_token->type == identifier_token) {
        id = identifier();
        if(!id) {
            return NULL;
        }
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

    if(read_token_and_lookahead(slash_token)) {
        is_self_closing = 1;
    }

    end_token = read_token_and_lookahead(closing_angle_token);
    if(!end_token) {
        return NULL;
    }

    result = malloc(sizeof(Node));
    result->type = jsx_opening_element_node;
    result->child = id;
    result->children = attributes;
    result->is_self_closing = is_self_closing;
    result->start = is_nested ? start_position : start_token->start;
    result->end = end_token->end;

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
    result->start = id->start;
    result->end = value->end;

    return result;
}

static Node *jsx_expression() {
    Node *result;
    Node **children = NULL;
    Token *start_token = NULL;
    Token *end_token = NULL;
    union jsx_child_function child_func;
    long start_position = -1;
    int i;

    start_token = read_token_and_lookahead(opening_curly_token);

    for(i = 0;; i++) {
        if(i == (MAX_LIST_SIZE - 1)) {
            fprintf(
                stderr,
                "Too many jsx expression parts. Max is %d.\n",
                MAX_LIST_SIZE
            );
            exit(1);
        }

        if(lookahead_token->type == opening_angle_token) {
            start_position = lookahead_token->start;
            lookahead();
            child_func.jsx_element_nested = jsx_element_nested;
        } else if(lookahead_token->type == closing_curly_token) {
            break;
        } else {
            child_func.other_one_argument_function = jsx_text;
        }

        if(!children) {
            children = malloc(sizeof(Node) * MAX_LIST_SIZE);
        }
        if(start_position > 0) {
            children[i] = child_func.jsx_element_nested(start_position);
        } else {
            children[i] = child_func.other_one_argument_function();
        }
    }
    if(children) {
        children[i] = NULL;
    }

    end_token = read_token_and_lookahead(closing_curly_token);

    result = malloc(sizeof(Node));
    result->type = jsx_expression_node;
    result->children = children;
    result->start = start_token->start;
    result->end = end_token->end;

    return result;
}

static Node *jsx_text() {
    Node *result;
    char *p;
    int skip_closing_curly_count = 0;

    result = malloc(sizeof(Node));
    result->type = jsx_text_node;
    result->value = malloc(JSX_MAX_TEXT_LENGTH);
    p = result->value;

    if(lookahead_token) {
        result->start = lookahead_token->start;
    }

    while(
        lookahead_token &&
        lookahead_token->type != opening_angle_token
    ) {
        int l = strlen(lookahead_token->value);
        int l_total = p - result->value;

        if(lookahead_token->type == opening_curly_token) {
            skip_closing_curly_count++;
        }
        if(lookahead_token->type == closing_curly_token) {
            if(skip_closing_curly_count) {
                skip_closing_curly_count--;
            } else {
                break;
            }
        }

        if(lookahead_token->type == string_token) {
            l_total += 2;
            *p = '\'';
            p++;
        }

        if(l_total >= (JSX_MAX_TEXT_LENGTH - 1)) {
            fprintf(
                stderr,
                "JSX text is too long. Max length is %d.\n",
                JSX_MAX_TEXT_LENGTH
            );
            exit(1);
        }

        strncpy(p, lookahead_token->value, l);
        p += l;
        if(lookahead_token->type == string_token) {
            *p = '\'';
            p++;
        }

        result->end = lookahead_token->end;
        free(lookahead_token->value);
        free(lookahead_token);
        lookahead_w_space_parsing();
    }

    *p = '\0';

    return result;
}

static Node *jsx_closing_element() {
    Node *result;
    Node *id = NULL;
    Token *start_token = NULL;
    Token *end_token = NULL;

    start_token = read_token_and_lookahead(slash_token);
    if(lookahead_token->type == identifier_token) {
        id = identifier();
    }
    end_token = read_token_and_lookahead(closing_angle_token);

    result = malloc(sizeof(Node));
    result->type = jsx_closing_element_node;
    result->child = id;
    result->start = start_token->start;
    result->end = end_token->end;

    return result;
}

static Node *identifier() {
    Node *result;
    Token *token;

    token = read_token_and_lookahead(identifier_token);
    if(!token) {
        return NULL;
    }
    result = malloc(sizeof(Node));
    result->type = identifier_node;
    result->value = token->value;
    result->start = token->start;
    result->end = token->end;

    return result;
}

static Node *string_literal() {
    Node *result;
    Token *token;

    result = malloc(sizeof(Node));
    result->type = string_literal_node;
    token = read_token_and_lookahead(string_token);
    result->value = token->value;
    result->start = token->start;
    result->end = token->end;

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
        return NULL;
        /* fprintf(stderr, "Unexpected token.\n");
        exit(1); */
    }

    lookahead();

    return token;
}
