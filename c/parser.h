enum node_type {
    program_node,
    numeric_literal_node,
    string_literal_node,
    expression_statement_node,
    statement_list_node
};

typedef struct Node {
    enum node_type type;
    int int_value;
    char *str_value;
    struct Node *child;
    struct Node **children;
} Node;

Node *parse();
