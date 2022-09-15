typedef union {
    Program* pm;
    NumericLiteral* nl;
} ASTNode;

void print_ast(Node* ast);
