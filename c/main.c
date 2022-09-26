#include "parser.h"
#include "utils.h"

int main() {
    Node *ast = parse();
    print_ast(ast);

    return 0;
}
