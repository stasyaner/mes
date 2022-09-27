#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "utils.h"

#define BUF_SIZE 255
#define JSON_NULL_STRING "null"

static void print_node(const Node *node, int nesting_level, char put_trailing_comma);
static void print_node_list(const Node **list, int nesting_level);

/* char *get_string_type_from_enum(enum node_type type) {
    switch(type) {
        case PROGRAM:
            return "Program";
        case NUMERIC_LITERAL:
            return "NumericLiteral";
        default:
            return "Unknown";
    }
} */

/* ??? get_node_value_by_type(Node *node) {
    switch(node->type) {
        case PROGRAM:
            return "Program";
        case NUMERIC_LITERAL:
            return "NumericLiteral";
        default:
            return "Unknown";
    }
} */

static char *get_tabs(int nesting_level) {
    char *tabs;
    int i;

    tabs = malloc(sizeof(char) * BUF_SIZE);

    for(i = 0; i < nesting_level && i < BUF_SIZE; i++) {
        tabs[i] = '\t';
    }
    tabs[i] = '\0';

    return tabs;
}

static void print_node(
    const Node *node,
    int nesting_level,
    char put_trailing_comma
) {
    char *tabs = get_tabs(nesting_level);

    if(!node) {
        printf(" null");
        if(put_trailing_comma) {
            printf(",\n");
        } else {
            printf("\n");
        }
        return;
    }

    switch(node->type) {
        case program_node:
            printf("%s\"type\": \"Program\",\n", tabs);
            printf("%s\"content\":", tabs);
            print_node(node->child, nesting_level, 1);
            break;
        case numeric_literal_node:
            printf(" {\n");
            printf("\t%s\"type\": \"NumericLiteral\",\n", tabs);
            printf("\t%s\"value\": %s\n", tabs, node->value);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case string_literal_node:
            printf(" {\n");
            printf("\t%s\"type\": \"StringLiteral\",\n", tabs);
            printf("\t%s\"value\": \"%s\"\n", tabs, node->value);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case expression_statement_node:
            printf("%s{\n", tabs);
            printf("\t%s\"type\": \"ExpressionStatement\",\n", tabs);
            printf("\t%s\"value\":", tabs);
            print_node(node->child, nesting_level + 1, 0);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case statement_list_node:
            printf(" [\n");
            print_node_list((const Node **)node->children, nesting_level + 1);
            printf("%s]\n", tabs);
            break;
        case binary_expression_node:
            printf(" {\n");
            printf("\t%s\"type\": \"BinaryExpression\",\n", tabs);
            printf("\t%s\"left\":", tabs);
            print_node(node->left, nesting_level + 1, 1);
            printf("\t%s\"right\":", tabs);
            print_node(node->right, nesting_level + 1, 1);
            printf("\t%s\"operator\": \"%s\"\n", tabs, node->operator);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case jsx_expression_node:
            printf(" {\n");
            printf("\t%s\"type\": \"JSXExpression\",\n", tabs);
            printf("\t%s\"openingElement\":", tabs);
            print_node(node->opening_element, nesting_level + 1, 1);
            printf("\t%s\"value\":", tabs);
            print_node(node->child, nesting_level + 1, 1);
            printf("\t%s\"closingElement\":", tabs);
            print_node(node->closing_element, nesting_level + 1, 0);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case jsx_opening_element_node:
            printf(" {\n");
            printf("\t%s\"type\": \"JSXOpeningElement\",\n", tabs);
            printf("\t%s\"isSelfClosing\": ", tabs);
            printf("%s\n", node->is_self_closing ? "true" : "false");
            printf("\t%s\"value\": \"%s\"\n", tabs, node->value);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case jsx_content_node:
            printf(" {\n");
            printf("\t%s\"type\": \"JSXContent\",\n", tabs);
            printf("\t%s\"value\": \"%s\"\n", tabs, node->value);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        case jsx_closing_element_node:
            printf(" {\n");
            printf("\t%s\"type\": \"JSXClosingElement\",\n", tabs);
            printf("\t%s\"value\": \"%s\"\n", tabs, node->value);
            if(put_trailing_comma) {
                printf("%s},\n", tabs);
            } else {
                printf("%s}\n", tabs);
            }
            break;
        default:
            printf(" {\n");
            printf("\t%s\"type\": \"Unknown\"\n", tabs);
            printf("%s}\n", tabs);
    }

    free(tabs);
}

static void print_node_list(const Node **list, int nesting_level) {
    int i;
    char is_last = 1;
    const Node *p;

    for(i = 0; (p = list[i]); i++) {
        if(list[i + 1] == NULL) {
            is_last = 0;
        }
        print_node(p, nesting_level, is_last);
    }
}

void print_ast(const Node *ast) {
    printf("Printing AST:\n");
    printf("{\n");
    print_node(ast, 1, 1);
    printf("}\n");
}

char is_number(int c) {
    return c >= '0' && c <= '9';
}

char is_double_quote(int c) {
    return c == '"';
}

char is_single_quote(int c) {
    return c == '\'';
}

char is_string_enclosure(int c) {
    return is_double_quote(c) || is_single_quote(c);
}

char is_space(int c) {
    return c == ' ';
}

char is_semicolon(int c) {
    return c == ';';
}

char is_linebreak(int c) {
    return c == '\n';
}

char is_opening_angle_bracket(int c) {
    return c == '<';
}

char is_closing_angle_bracket(int c) {
    return c == '>';
}
char is_slash(int c) {
    return c == '/';
}
