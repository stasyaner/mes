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
    equality_token
};

typedef struct {
    enum token_type type;
    char *value;
} Token;

char check_token_type(Token *token, enum token_type acceptable_token_type);
Token *get_next_token();
