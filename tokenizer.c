#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 255

static int c_cached = '\0';

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
    if(is_number(c)) {
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
        for(i = 0;; i++) {
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
            fprintf(
                stderr,
                "String can't be enclosed into different quotes.\n"
            );
            exit(1);
        }
    } else if(is_semicolon(c)) {
        token->type = semicolon_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_opening_angle_bracket(c)) {
        token->type = opening_angle_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_closing_angle_bracket(c)) {
        token->type = closing_angle_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_slash(c)) {
        token->type = slash_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_opening_curly(c)) {
        token->type = opening_curly_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_closing_curly(c)) {
        token->type = closing_curly_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_pipe(c)) {
        token->type = pipe_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_ampersand(c)) {
        token->type = ampersand_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_equality(c)) {
        token->type = equality_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if(is_alpha(c)) {
        token->type = identifier_token;
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
            if(is_alpha(c) || is_underscore(c)) {
                token->value[i] = c;
            } else {
                c_cached = c;
                break;
            }
        }
        token->value[i] = '\0';
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
