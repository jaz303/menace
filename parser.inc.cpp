// TODO: this COPY_TOKEN() stuff is terrible. should just be passing
// pointer to the lexer.

struct parsed_token {
	int token;
	int line, column;
	int tok_start, tok_len;
	char *tok;
};

typedef struct mc_parser {
	mc_lexer_t lexer;
	parsed_token curr, next;
	const char *error;
} mc_parser_t;

// val_t parse_prefix_op(mc_parser_t*);
// val_t parse_paren_exp(mc_parser_t*);
// val_t parse_infix_op(mc_parser_t*, val_t);
// val_t parse_call(mc_parser_t*, val_t);
// val_t parse_statements(mc_parser_t*, int);
// val_t parse_expression(mc_parser_t*, int);

#define COPY_TOKEN(lexer, target) \
	(target).token = (lexer).token; \
	(target).line = (lexer).line; \
	(target).column = (lexer.column); \
	(target).tok_start = (lexer).tok_start; \
	(target).tok_len = (lexer).tok_len; \
	(target).tok = (lexer).tok

#define SKIP_NL() \
	while (AT(TOK_NL)) { NEXT(); }

#define PARSE_INTO(var, rule, ...) \
	var = parse_##rule(p, ## __VA_ARGS__); \
	if (p->error) return mk_nil()

#define PARSE(var, rule, ...) \
	val_t var; \
	PARSE_INTO(var, rule, ## __VA_ARGS__)

#define PARSE_STATEMENTS(var, term) \
	val_t var = parse_statements(p, term); \
	if (p->error) return mk_nil()

#define PARSE_STATEMENT(var, term) \
	val_t var = parse_statement(p, term); \
	if (p->error) return mk_nil()

#define ERROR(msg) \
	p->error = msg; \
	return mk_nil()

#define ACCEPT(tok) \
	if (!AT(tok)) { ERROR("expected: " #tok); } \
	NEXT()

#define NEXT() \
	p->curr = p->next; \
	mc_lexer_next(&p->lexer); \
	COPY_TOKEN(p->lexer, p->next)

#define CURR() \
	(p->curr.token)

#define TEXT() \
	(p->curr.tok)

#define TEXT_LEN() \
	(p->curr.tok_len)

#define AT(tok) \
	(CURR() == tok)

#define IDENT() mk_ident(mc_intern(TEXT(), TEXT_LEN()))

#define PEEK() \
	(p->next.token)

#define MK2(type, arg1, arg2) \
	mk_ast_##type(arg1, arg2)

val_t parse_ident(mc_parser_t *p) {
	int sym;
	if (AT(TOK_IDENT)) {
		sym = mc_intern(TEXT(), TEXT_LEN());
	}
	ACCEPT(TOK_IDENT);
	return mk_ident(sym);
}

val_t parse_expression(mc_parser_t *p);

val_t parse_string(mc_parser_t *p) {
	val_t str = mk_string_from_token(p->lexer.tok, p->lexer.tok_len);
	NEXT();
	return str;
}

val_t parse_int(mc_parser_t *p) {
	// TODO: overflow
	// TODO: use correct int type
	int val = 0;
	for (int i = 0; i < p->lexer.tok_len; ++i) {
		val = (val * 10) + (p->lexer.tok[i] - '0');
	}
	NEXT();
	return mk_int(val);
}

val_t parse_atom(mc_parser_t *p) {
	val_t out;
	if (AT(TOK_LPAREN)) {
		NEXT();
		PARSE_INTO(out, expression);
		ACCEPT(TOK_RPAREN);
	} else if (AT(TOK_INT)) {
		PARSE_INTO(out, int);
	} else if (AT(TOK_STRING)) {
		PARSE_INTO(out, string);
	} else if (AT(TOK_IDENT)) {
		PARSE_INTO(out, ident);
	} else {
		ERROR("expected: atom");
	}
	return out;
}

val_t parse_argument(mc_parser_t *p) {
	return parse_atom(p);
}

val_t parse_message(mc_parser_t *p) {
	if (!AT(TOK_IDENT)) {
		ERROR("expected: identifier");
	}
	if (PEEK() == TOK_COLON) {
		val_t head = mk_nil(), tail;
		while (AT(TOK_IDENT)) {
			val_t name = IDENT();
			NEXT();
			ACCEPT(TOK_COLON);
			PARSE(arg, argument);
			val_t msg_part = mk_ast_msg_part(name, arg);
			val_t node = mk_ast_list(msg_part, mk_nil());
			if (nil_p(head)) {
				head = tail = node;
			} else {
				((ast_list_t*)tail.ast)->next = node;
				tail = node;
			}
		}
		return head;
	} else {
		val_t sel = IDENT();
		NEXT();
		return mk_ast_unary_msg(sel);
	}
	return mk_nil();
}

val_t parse_expression(mc_parser_t *p) {
	val_t exp;
	PARSE_INTO(exp, atom);
	if (AT(TOK_IDENT)) {
		PARSE(msg, message);
		exp = mk_ast_send(exp, msg);
	}
	return exp;
}

val_t parse_assign(mc_parser_t *p) {
	if (!AT(TOK_IDENT)) {
		ERROR("expected: identifier");
	}
	val_t assignee = mk_ident(mc_intern(TEXT(), TEXT_LEN()));
	NEXT();
	ACCEPT(TOK_ASSIGN);
	PARSE(exp, expression);
	return mk_ast_assign(assignee, exp);
}

val_t parse_statement(mc_parser_t *p) {
	val_t stmt;
	if (AT(TOK_IDENT) && PEEK() == TOK_ASSIGN) {
		PARSE_INTO(stmt, assign);
	} else {
		PARSE_INTO(stmt, expression);
	}
	return stmt;
}

/* Public Interface */

void mc_parser_init(mc_parser_t *parser) {
	mc_lexer_next(&parser->lexer);
	COPY_TOKEN(parser->lexer, parser->curr);
	mc_lexer_next(&parser->lexer);
	COPY_TOKEN(parser->lexer, parser->next);
	parser->error = NULL;
}

val_t mc_parse_repl_line(mc_parser_t *p) {
	PARSE(stmt, statement);
	ACCEPT(TOK_NL);
	return stmt;
}

#undef SKIP_NL
#undef PARSE_INTO
#undef PARSE
#undef PARSE_STATEMENTS
#undef PARSE_STATEMENT
#undef ERROR
#undef ACCEPT
#undef NEXT
#undef CURR
#undef TEXT
#undef TEXT_LEN
#undef AT
#undef MK2