enum node_type {
    file_node,
    numeric_literal_node,
    string_literal_node,
    identifier_node,
    expression_statement_node,
    statement_list_node,
    binary_expression_node,
    jsx_element_node,
    jsx_opening_element_node,
    jsx_attribute_node,
    jsx_expression_node,
    jsx_text_node,
    jsx_closing_element_node
};

typedef struct ASTNode {
    enum node_type type;
    long start;
    long end;
    char *value;
    char *operator;
    char is_self_closing;
    struct ASTNode *child;
    struct ASTNode **children;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *opening_element;
    struct ASTNode *closing_element;
} Node;

Node *parse();
