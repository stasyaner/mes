enum node_type { program_node, numeric_literal_node, string_literal_node };

typedef struct Node {
    enum node_type type;
    int int_value;
    char *str_value;
    struct Node *child;
} Node;

Node *parse();
Node *program();
Node *numeric_literal();
char *read_token_and_lookahead(int acceptable_token_type);
