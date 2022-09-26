#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 255

static int c_cached = '\0';
static char is_jsx_opened = 0;

Token *get_next_token() {
    Token *token;
    char c;
    char c_stored;
    int i;

    if(c_cached != '\0') {
        c = c_cached;
        c_cached = '\0';
    } else {
        c = getchar();
    }

    token = malloc(sizeof(Token));
    token->value = malloc(sizeof(char) * BUF_SIZE);
    if(is_number(c)) {
        token->type = number_token;
        token->value[0] = c;
        for(i = 1; is_number(c = getchar()) && i < BUF_SIZE; i++) {
            token->value[i] = c;
        }
        token->value[i + 1] = '\0';
        c_cached = c;
    } else if(is_string_enclosure(c)) {
        token->type = string_token;
        c_stored = c;
        for(i = 0; !is_string_enclosure(c = getchar()) && i < BUF_SIZE; i++) {
            token->value[i] = c;
        }
        token->value[i + 1] = '\0';
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
        c_stored = c;
        c = getchar();
        if(is_slash(c)) {
            token->type = jsx_closing_token;
        } else if(is_space(c)) {
            token->type = relational_token;
            token->value[0] = c_stored;
            token->value[1] = '\0';
        } else {
            token->type = jsx_opening_token;
            token->value[0] = c;
            is_jsx_opened = 1;
        }

        if(token->type != relational_token) {
            for(
                i = 1;
                !is_closing_angle_bracket(c = getchar()) && i < BUF_SIZE;
                i++
            ) {
                if(is_slash(c)) {
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
                }
                token->value[i] = c;
            }
            /* This is to cut the trailing space. It's hacky, but when
               we implement jsx props, everything is gonna be different
               here anyway. */
            if(token->type == jsx_self_closing_token) {
                token->value[i - 1] = '\0';
            } else {
                token->value[i + 1] = '\0';
            }
        }
    } else if(is_space(c) || is_linebreak(c)) {
        return get_next_token();
    } else {
        /* Supposed to mean EOF "token", but maybe sould throw
           "unrecognized token" error? */
        return NULL;
    }

    return token;
}

char check_token_type(Token *token, int acceptable_token_type) {
    if(token == NULL) {
        fprintf(stderr, "Unexpected end of input.\n");
        exit(1);
    }

    if(token->type != acceptable_token_type) {
        /* fprintf(stderr, "Unexpected token.\n"); */

        return 1;
    }

    return 0;
}
