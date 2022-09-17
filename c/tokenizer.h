enum token_type { string_token, number_token };
typedef struct {
    enum token_type type;
    char *value;
} Token;

void assert_token_type(Token *token, int acceptable_token_type);
Token *get_next_token();
