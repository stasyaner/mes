#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 256

Token *get_next_token() {
    Token *token;
    char c = getchar();
    char c_stored;
    int i;

    token = malloc(sizeof(Token));
    token->value = malloc(sizeof(char) * BUF_SIZE);
    if(is_number(c)) {
        token->type = number_token;
        token->value[0] = c;
        for(i = 1; is_number(c = getchar()) && i <= BUF_SIZE; i++) {
            token->value[i] = c;
        }
    } else if (is_string_enclosure(c)) {
        token->type = string_token;
        c_stored = c;
        for(i = 0; !is_string_enclosure(c = getchar()) && i <= BUF_SIZE; i++) {
            token->value[i] = c;
        }
        token->value[i+1] = '\0';
        if(c != c_stored) {
            fprintf(stderr,
                    "String can't be enclosed into different quotes.\n");
            exit(1);
        }
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
