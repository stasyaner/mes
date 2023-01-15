#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "utils.h"

#define BUF_SIZE 20000

static char *get_input() {
    char *buf = NULL;
    long i = 0;
    char c = EOF;

    while((c = getchar()) != EOF) {
        if(!buf) {
            buf = malloc(BUF_SIZE);
        }

        if(i == (BUF_SIZE - 2)) {
            fprintf(stderr, "Buffer is too small.\n");
            exit(1);
        }

        buf[i] = c;
        i++;
    }

    buf[i] = '\0';

    return buf;
}

int main() {
    char *input = NULL;
    Node *ast = NULL;

    input = get_input();
    ast = parse(input);
    print_ast(ast);

    return 0;
}
