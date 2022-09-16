enum node_type { PROGRAM, NUMERIC_LITERAL };

typedef struct Node {
    enum node_type type;
    int int_value;
    char* str_value;
    struct Node* child;
} Node;

Node* parse();
Node* program();
Node* numeric_literal();
