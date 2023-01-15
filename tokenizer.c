#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 255

static char *input = NULL;
static long cursor = 0;
static int c_cached = '\0';
static long last_char_index = 0;

/* typedef struct Specification {
    char (*qualification_func)(int);
    Token *(*handler_func)(int);
} Spec;
Token *handle_number(int c);
static const Spec spec[] = {
    { is_number, handle_number }
}; */

static char getchar_from_input() {
    char c = input[cursor];

    if(c == '\0') {
        return EOF;
    }

    cursor++;

    return c;
}

void tokenizer_init(char *init_input) {
    if(!init_input) {
        fprintf(stderr, "Trying to init tokenier with NULL input.");
        exit(1);
    }

    input = init_input;
}

Token *get_next_token_base(char parse_space) {
    Token *token;
    char c;
    int i;

    if(c_cached != '\0') {
        c = c_cached;
        c_cached = '\0';
    } else {
        c = getchar_from_input();
    }

    token = malloc(sizeof(Token));
    token->value = malloc(BUF_SIZE);
    token->start = last_char_index;

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
            c = getchar_from_input();
            if(is_number(c)) {
                token->value[i] = c;
            } else {
                break;
            }
        }
        token->value[i] = '\0';
        token->end = token->start + i;
        last_char_index = token->end;
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
            c = getchar_from_input();
            if(is_string_enclosure(c)) {
                break;
            } else {
                token->value[i] = c;
            }
        }
        token->value[i] = '\0';
        /* i+2 = +1 as we started from 0 and +1 for closing quote */
        token->end = token->start + i + 2;
        last_char_index = token->end;
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
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_opening_angle_bracket(c)) {
        token->type = opening_angle_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_closing_angle_bracket(c)) {
        token->type = closing_angle_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_slash(c)) {
        token->type = slash_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_opening_curly(c)) {
        token->type = opening_curly_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_closing_curly(c)) {
        token->type = closing_curly_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_pipe(c)) {
        token->type = pipe_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_ampersand(c)) {
        token->type = ampersand_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
    } else if(is_equality(c)) {
        token->type = equality_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
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
            c = getchar_from_input();
            if(is_alpha(c) || is_underscore(c) || is_number(c)) {
                token->value[i] = c;
            } else {
                c_cached = c;
                break;
            }
        }
        token->value[i] = '\0';
        token->end = token->start + i;
        last_char_index = token->end;
    } else if(is_space(c)) {
        if(parse_space) {
            token->type = space_token;
            token->value[0] = c;
            token->value[1] = '\0';
            for(i = 1;; i++) {
                c = getchar_from_input();
                if(is_space(c)) {
                    continue;
                } else {
                    c_cached = c;
                    break;
                }
            }
            token->end = token->start + i;
            last_char_index = token->end;
        } else {
            free(token->value);
            free(token);
            last_char_index++;
            return get_next_token_base(parse_space);
        }
    } else if(is_linebreak(c)) {
        free(token->value);
        free(token);
        last_char_index++;
        return get_next_token_base(parse_space);
    } else if(is_eof(c)) {
        free(token->value);
        free(token);
        last_char_index++;
        return NULL;
    } else {
        token->type = special_token;
        token->value[0] = c;
        token->value[1] = '\0';
        token->end = token->start + 1;
        last_char_index = token->end;
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
