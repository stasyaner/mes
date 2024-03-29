#ifndef __MES_UTILS_H
#define __MES_UTILS_H

#include "parser.h"

void print_ast(const Node *ast);
char is_number(int c);
char is_double_quote(int c);
char is_single_quote(int c);
char is_string_enclosure(int c);
char is_space(int c);
char is_semicolon(int c);
char is_linebreak(int c);
char is_opening_angle_bracket(int c);
char is_closing_angle_bracket(int c);
char is_slash(int c);
char is_alpha(int c);
char is_underscore(int c);
char is_opening_curly(int c);
char is_closing_curly(int c);
char is_pipe(int c);
char is_ampersand(int c);
char is_equality(int c);

#endif
