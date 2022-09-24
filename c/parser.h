enum node_type {
    program_node,
    numeric_literal_node,
    string_literal_node,
    expression_statement_node,
    statement_list_node,
    binary_expression_node
};

typedef struct ASTNode {
    enum node_type type;
    /* void ptr? */
    int int_value;
    char *str_value;
    struct ASTNode *child;
    struct ASTNode **children;
    struct ASTNode *left;
    struct ASTNode *right;
    char *operator;
} Node;

Node *parse();
