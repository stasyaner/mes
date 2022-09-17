#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

#define BUF_SIZE 256

Token *get_next_token() {
    Token *token;
    char token_value[BUF_SIZE];
    char c = getchar();
    int i;

    token = malloc(sizeof(Token));
    if(isNumber(c)) {
        token->type = number_token;
        token_value[0] = c;
        for(i = 1; isNumber(c = getchar()) && i <= BUF_SIZE; i++) {
            token_value[i] = c;
        }
        token->value = token_value;
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
