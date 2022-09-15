enum node_type { PROGRAM, NUMERIC_LITERAL };

typedef struct {
    const char* type;
    int value;
} NumericLiteral;

typedef struct {
    const char* type;
    const NumericLiteral* value;
} Program;

typedef struct Node {
    enum node_type type;
    int int_value;
    char* str_value;
    struct Node* child;
} Node;

Node* parse();
Node* program();
Node* numericLiteral();
