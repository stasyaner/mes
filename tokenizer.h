enum token_type {
    string_token,
    number_token,
    semicolon_token,
    relational_token,
    jsx_opening_token,
    jsx_closing_token,
    jsx_self_closing_token,
    jsx_text_token
};

typedef struct {
    enum token_type type;
    char *value;
} Token;

char check_token_type(Token *token, enum token_type acceptable_token_type);
Token *get_next_token();
