#ifndef __MES_TOKENIZER_H
#define __MES_TOKENIZER_H

enum token_type {
    string_token,
    number_token,
    semicolon_token,
    opening_angle_token,
    closing_angle_token,
    identifier_token,
    slash_token,
    opening_curly_token,
    closing_curly_token,
    ampersand_token,
    pipe_token,
    equality_token,
    space_token,
    linebreak_token,
    special_token
};

typedef struct {
    enum token_type type;
    char *value;
    long start;
    long end;
} Token;

char check_token_type(Token *token, enum token_type acceptable_token_type);
void tokenizer_init(char *init_input);
Token *get_next_token_base(char parse_space, char parse_linebreak);
#define get_next_token_w_space_linebreak() get_next_token_base(1, 1)
#define get_next_token_w_linebreak() get_next_token_base(0, 1)
#define get_next_token_w_space() get_next_token_base(1, 0)
#define get_next_token() get_next_token_base(0, 0)

#endif
