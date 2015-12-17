enum {
    __UNUSED_TOKEN__ = 0,

    TOK_ASSIGN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LPAREN,
    TOK_RPAREN,

    TOK_OP_MAX,

    TOK_INT,
    TOK_IDENT,
    TOK_STRING,

    TOK_TRUE,
    TOK_FALSE,

    TOK_NL,
    TOK_COMMA,

    TOK_TERMINAL = 1000,
    TOK_EOF,
    TOK_ERROR
};

typedef struct mc_lexer {
    char *text;
    size_t pos;
    int line;
    int column;
    int tok_start;
    int tok_len;
    char *tok;
    const char *error;
} mc_lexer_t;

#define MARK() l->tok_start = l->pos; l->tok = &(l->text[l->pos])
#define END() l->tok_len = l->pos - l->tok_start
#define EMIT(tok) return tok
#define CURR() (l->text[l->pos])
#define NEXT() lexer_next(l)
#define LEN() (l->tok_len)
#define TEXTEQ(str) streql(str, l->tok, l->tok_len)
#define ERROR(msg) \
    l->error = msg; \
    return TOK_ERROR

int space_p(char c) {
    return c == ' ' || c == '\t';
}

int digit_p(char c) {
    return c >= '0' && c <= '9';
}

int ident_start_p(char c) {
    return (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || (c == '_');
}

int ident_rest_p(char c) {
    return ident_start_p(c) || digit_p(c);
}

void lexer_next(mc_lexer_t *l) {
    // TODO: handle \r\n
    if (l->text[l->pos] == '\n') {
        l->line++;
        l->column = 1;
    } else {
        l->column++;
    }
    l->pos++;
}

void rt_lexer_init(mc_lexer_t *lexer, char *text) {
    lexer->text = text;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->tok_start = 0;
    lexer->tok_len = 0;
    lexer->tok = NULL;
    lexer->error = NULL;
}

void rt_lexer_clone(mc_lexer_t *d, const mc_lexer_t *s) {
    d->text = s->text;
    d->pos = s->pos;
    d->line = s->line;
    d->column = s->column;
    d->tok_start = s->tok_start;
    d->tok_len = s->tok_len;
    d->tok = s->tok;
    d->error = s->error;
}

int rt_lexer_next(mc_lexer_t *l) {
    if (l->error) {
        return TOK_ERROR;
    }
    while (space_p(l->text[l->pos])) {
        l->pos++;
    }
    switch (CURR()) {
        case '\0': EMIT(TOK_EOF);
        case '\r':
            NEXT();
            if (CURR() == '\n') {
                NEXT();
            }
            EMIT(TOK_NL);
        case '\n': NEXT(); EMIT(TOK_NL);
        case '(': NEXT(); EMIT(TOK_LPAREN);
        case ')': NEXT(); EMIT(TOK_RPAREN);
        case '[': NEXT(); EMIT(TOK_LBRACKET);
        case ']': NEXT(); EMIT(TOK_RBRACKET);
        case ',': NEXT(); EMIT(TOK_COMMA);
        case ':':
            NEXT();
            if (CURR() == '=') {
                NEXT();
                EMIT(TOK_ASSIGN);
            } else {
                ERROR("expected: '='");
            }
        case '"':
            {
                MARK(); NEXT();
                int state = 0;
                while (1) {
                    if (CURR() == '\0') {
                        ERROR("unexpected EOF");
                    }
                    switch (state) {
                        case 0:
                            if (CURR() == '\'') {
                                state = 1;
                            } else if (CURR() == '"') {
                                NEXT();
                                END();
                                EMIT(TOK_STRING);
                            }
                            break;
                        case 1:
                            state = 0;
                            break;
                    }
                    NEXT();
                }
            }
        default:
            if (ident_start_p(CURR())) {
                MARK(); NEXT();
                while (ident_rest_p(CURR())) {
                    NEXT();
                }
                END();
                if (TEXTEQ("true"))     EMIT(TOK_TRUE);
                if (TEXTEQ("false"))    EMIT(TOK_FALSE);
                EMIT(TOK_IDENT);
            } else if (digit_p(CURR())) {
                MARK(); NEXT();
                while (digit_p(CURR())) {
                    NEXT();
                }
                END();
                EMIT(TOK_INT);
            } else {
                ERROR("unexpected character in input");
            }
    }
}

#undef MARK
#undef END
#undef EMIT
#undef CURR
#undef NEXT
#undef LEN
#undef TEXTEQ
#undef ERROR