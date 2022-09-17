#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "utils.h"

int main() {
    Node *ast = parse();
    print_ast(ast);
    free(ast);

    return 0;
}
