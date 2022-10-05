#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 255

static int c_cached = '\0';
static char is_jsx_opened = 0;

/* typedef struct Specification {
    char (*qualification_func)(int);
    Token *(*handler_func)(int);
} Spec;
Token *handle_number(int c);
static const Spec spec[] = {
    { is_number, handle_number }
}; */

Token *get_next_token() {
    Token *token;
    char c;
    int i;

    if(c_cached != '\0') {
        c = c_cached;
        c_cached = '\0';
    } else {
        c = getchar();
    }

    token = malloc(sizeof(Token));
    token->value = malloc(BUF_SIZE);
    if(is_jsx_opened) {
        token->type = jsx_text_token;
        token->value[0] = c;
        for(i = 1;; i++) {
            if(i == (BUF_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Token is too long. Max length is %d.\n",
                    BUF_SIZE
                );
                exit(1);
            }
            c = getchar();
            if(is_opening_angle_bracket(c)) {
                c_cached = c;
                break;
            }
            token->value[i] = c;
            /* if (isEOF(c)) {
                throw new Error(
                    'Unexpected end of input while reading JSX text.'
                );
            } */
        }
        token->value[i] = '\0';
        is_jsx_opened = 0;
    } else if(is_number(c)) {
        token->type = number_token;
        token->value[0] = c;
        for(i = 1;; i++) {
            if(i == (BUF_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Token is too long. Max length is %d.\n",
                    BUF_SIZE
                );
                exit(1);
            }
            c = getchar();
            if(is_number(c)) {
                token->value[i] = c;
            } else {
                break;
            }
        }
        token->value[i] = '\0';
        c_cached = c;
    } else if(is_string_enclosure(c)) {
        char c_stored = c;
        token->type = string_token;
        for(i = 0; !is_string_enclosure(c = getchar()) && i < BUF_SIZE; i++) {
            if(i == (BUF_SIZE - 1)) {
                fprintf(
                    stderr,
                    "Token is too long. Max length is %d.\n",
                    BUF_SIZE
                );
                exit(1);
            }
            c = getchar();
            if(is_string_enclosure(c)) {
                break;
            } else {
                token->value[i] = c;
            }
        }
        token->value[i] = '\0';
        if(c != c_stored) {
            fprintf(stderr,
                    "String can't be enclosed into different quotes.\n");
            exit(1);
        }
    } else if(is_semicolon(c)) {
        token->type = semicolon_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_closing_angle_bracket(c)) {
        token->type = relational_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_opening_angle_bracket(c)) {
        char c_stored = c;
        c = getchar();
        if(is_slash(c)) {
            token->type = jsx_closing_token;
            i = 0;
            is_jsx_opened = 0;
        } else if(is_alpha(c)) {
            token->type = jsx_opening_token;
            token->value[0] = c;
            i = 1;
            is_jsx_opened = 1;
        } else if(is_space(c)) {
            token->type = relational_token;
            token->value[0] = c_stored;
            token->value[1] = '\0';
        }

        if(token->type != relational_token) {
            for(;; i++) {
                if(i == (BUF_SIZE - 1)) {
                    fprintf(
                        stderr,
                        "Token is too long. Max length is %d.\n",
                        BUF_SIZE
                    );
                    exit(1);
                }
                c = getchar();
                if(is_closing_angle_bracket(c)) {
                    break;
                } else if(is_slash(c)) {
                    if(token->type == jsx_closing_token) {
                        fprintf(stderr,
                                "Unexpected \"/\" while reading"
                                " closing JSX token.\n");
                        exit(1);
                    }
                    getchar();
                    token->type = jsx_self_closing_token;
                    is_jsx_opened = 0;
                    break;
                } else {
                    token->value[i] = c;
                }
            }
            /* This is to cut the trailing space. It's hacky, but when
               we implement jsx props, everything is gonna be different
               here anyway. */
            if(token->type == jsx_self_closing_token) {
                token->value[i - 1] = '\0';
            } else {
                token->value[i] = '\0';
            }
        }
    } else if(is_space(c) || is_linebreak(c)) {
        free(token->value);
        free(token);
        return get_next_token();
    } else {
        return NULL;
    }

    return token;
}

char check_token_type(Token *token, enum token_type acceptable_token_type) {
    if(!token) {
        fprintf(stderr, "Unexpected end of input.\n");
        exit(1);
    }

    if(token->type != acceptable_token_type) {
        /* fprintf(stderr, "Unexpected token.\n"); */

        return 1;
    }

    return 0;
}
