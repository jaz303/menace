val_t mk_nil() {
    val_t out = { .type = T_NIL };
    return out;
}

val_t mk_true() {
    val_t out = { .type = T_TRUE };
    return out;
}

val_t mk_false() {
    val_t out = { .type = T_FALSE };
    return out;
}

val_t mk_int(int val) {
    val_t out;
    out.type = T_INT;
    out.ival = val;
    return out;
}

val_t mk_ident(int id) {
    val_t out;
    out.type = T_IDENT;
    out.ival = id;
    return out;
}

val_t mk_string(mc_string_t *str) {
    val_t out;
    out.type = T_STRING;
    out.str = str;
    return out;
}

// TODO: need to think about string allocation
val_t mk_string_from_token(const char *tok, int tok_len) {
    const int tok_start = 1;
    const int tok_end = tok_len - 1;

    // count string length
    int length = 0, state = 0;
    for (int i = tok_start; i < tok_end; ++i) {
        if (state == 0 && tok[i] == '\\') {
            state = 1;
        } else {
            length++;
            state = 0;
        }
    }

    // allocate storage
    mc_string_t *str = (mc_string_t*)malloc(sizeof(mc_string_t) + (sizeof(char) * (length + 1)));
    if (str == NULL) {
        // TODO: error
        return mk_nil();
    }

    // copy decoded string
    state = 0;
    int ix = 0;
    for (int i = tok_start; i < tok_end; ++i) {
        if (state == 0) {
            if (tok[i] == '\\') {
                state = 1;
            } else {
                str->str[ix++] = tok[i];
            }
        } else {
            switch (tok[i]) {
                case 'n': str->str[ix++] = '\n'; break;
                case 'r': str->str[ix++] = '\r'; break;
                case 't': str->str[ix++] = '\t'; break;
                default:
                    fprintf(stderr, "BUG: illegal string escape character leaked to decoder\n");
                    exit(1);
            }
        }
    }

    str->length = length;
    str->str[length] = 0;

    return mk_string(str);
}

int nil_p(val_t v) {
    return v.type == T_NIL;
}

int truthy_p(val_t v) {
    return (v.type != T_NIL) && (v.type != T_FALSE);
}