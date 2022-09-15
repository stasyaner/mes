#include <stdio.h>
#include "parser.h"
#include "utils.h"

/* char* get_string_type_from_enum(enum node_type type) {
    switch(type) {
        case PROGRAM:
            return "Program";
        case NUMERIC_LITERAL:
            return "NumericLiteral";
        default:
            return "Unknown";
    }
} */

/* ??? get_node_value_by_type(Node* node) {
    switch(node->type) {
        case PROGRAM:
            return "Program";
        case NUMERIC_LITERAL:
            return "NumericLiteral";
        default:
            return "Unknown";
    }
} */

void print_ast(Node* ast) {
    Node* p = ast;
    printf("Printing AST:\n");
    printf("{\n");
    while(p != NULL) {
        switch(p->type) {
            case PROGRAM:
                printf("\ttype: Program\n");
                printf("\tvalue:");
                break;
            case NUMERIC_LITERAL:
                printf("\t\ttype: NumericLiteral\n");
                printf("\t\tvalue: %d\n", p->int_value);
                printf("\t}\n");
                break;
            default:
                printf("\ttype: Unknown\n");
        }
        if(p->child != NULL) {
            p = p->child;
        } else {
            p = NULL;
        }
    }
    printf("}\n");
}
