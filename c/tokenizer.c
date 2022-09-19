#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 255

int cached_c = '\0';

Token *get_next_token() {
    Token *token;
    char c;
    char c_stored;
    int i;

    if(cached_c != '\0') {
        c = cached_c;
        cached_c = '\0';
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
        cached_c = c;
    } else if (is_string_enclosure(c)) {
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
    } else if (is_semicolon(c)) {
        token->type = semicolon_token;
        token->value[0] = c;
        token->value[1] = '\0';
    } else if (is_space(c) || is_linebreak(c)) {
        return get_next_token();
    } else {
        return NULL;
    }

    return token;
}

void assert_token_type(Token *token, int acceptable_token_type) {
    if(token == NULL) {
        fprintf(stderr, "Unexpected end of input.\n");
        exit(1);
    }

    if(token->type != acceptable_token_type) {
        fprintf(stderr, "Unexpected token.\n");
        exit(1);
    }
}
