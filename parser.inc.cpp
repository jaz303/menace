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

val_t parse_message(mc_parser_t *p) {
	return mk_nil();
}

val_t parse_expression(mc_parser_t *p) {
	val_t exp;
	PARSE_INTO(exp, atom);
	if (AT(TOK_IDENT)) {
		PARSE(msg, message);
		// exp = mk_ast_send(exp, msg);
	}
	return exp;
}

val_t parse_assign(mc_parser_t *p) {
	ACCEPT(TOK_IDENT);
	ACCEPT(TOK_ASSIGN);
	PARSE(exp, expression);
	val_t foo;
	return foo;
}

val_t parse_statement(mc_parser_t *p) {
	val_t stmt;
	if (AT(TOK_IDENT)) {
		if (PEEK() == TOK_ASSIGN) {
			PARSE_INTO(stmt, assign);
		} else {
			PARSE_INTO(stmt, expression);
		}
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

val_t mc_parse_repl_line(mc_parser_t *parser) {
	return mk_nil();
}

// val_t parse_expression_list(mc_parser_t *p) {
// 	PDEBUG("> expression list");
// 	val_t head = mk_nil(), tail = mk_nil();
// 	while (1) {
// 		PARSE(exp, expression, 0);
// 		val_t node = mk_ast_list(exp, mk_nil());
// 		if (nil_p(head)) {
// 			head = tail = node;
// 		} else {
// 			((ast_list_t*)tail.ast)->next = node;
// 			tail = node;
// 		}
// 		if (AT(TOK_COMMA)) {
// 			NEXT();
// 		} else {
// 			break;
// 		}
// 	}
// 	PDEBUG("< expression list");
// 	return head;
// }

// val_t parse_call(mc_parser_t *p, val_t left) {
// 	PDEBUG("> call");
// 	ACCEPT(TOK_LPAREN);
// 	val_t args;
// 	if (AT(TOK_RPAREN)) {
// 		args = mk_nil();
// 	} else {
// 		PARSE_INTO(args, expression_list);
// 	}
// 	ACCEPT(TOK_RPAREN);
// 	PDEBUG("< call");
// 	return mk_ast_call(left, args);
// }







// val_t parse_prefix_op(mc_parser_t *p) {
// 	PDEBUG("> prefix op");
// 	int optok = CURR();
// 	NEXT();
// 	PARSE(exp, expression, 0);
// 	PDEBUG("< prefix op");
// 	return mk_ast_unop(prefix_ops[optok].op, exp);
// }

// val_t parse_paren_exp(mc_parser_t *p) {
// 	PDEBUG("> paren exp");
// 	ACCEPT(TOK_LPAREN);
// 	PARSE(exp, expression, 0);
// 	ACCEPT(TOK_RPAREN);
// 	PDEBUG("< paren exp");
// 	return exp;
// }

// val_t parse_infix_op(mc_parser_t *p, val_t left) {
// 	PDEBUG("> infix op");
// 	int optok = CURR();
// 	int next_precedence = infix_ops[optok].precedence
// 							- (infix_ops[optok].right_associative ? 1 : 0);
// 	if (next_precedence < 0) {
// 		ERROR("illegal precedence value; this is a bug.");
// 	}
// 	NEXT();
// 	PARSE(right, expression, next_precedence);
// 	PDEBUG("< infix op");
// 	return mk_ast_binop(infix_ops[optok].op, left, right);
// }

// val_t parse_expression(mc_parser_t *p, int precedence) {
// 	PDEBUG("> expression");

// 	val_t left;
// 	if (AT(TOK_IDENT)) {
// 		PARSE_INTO(left, ident);
// 	} else if (AT(TOK_TRUE)) {
// 		left = mk_true();
// 		NEXT();
// 	} else if (AT(TOK_FALSE)) {
// 		left = mk_false();
// 		NEXT();
// 	} else if (AT(TOK_STRING)) {
// 		PARSE_INTO(left, string);
// 	} else if (AT(TOK_INT)) {
// 		PARSE_INTO(left, int);
// 	} else if ((CURR() < TOK_OP_MAX)
// 				&& (prefix_ops[CURR()].parser != NULL)) {
// 		left = prefix_ops[CURR()].parser(p);
// 		if (p->error) return mk_nil();
// 	} else {
// 		// TODO: better error message
// 		ERROR("parse error");
// 	}

// 	while ((CURR() < TOK_OP_MAX)
// 			&& (infix_ops[CURR()].parser != NULL)
// 			&& (precedence < infix_ops[CURR()].precedence)) {
// 		left = infix_ops[CURR()].parser(p, left);
// 		if (p->error) return mk_nil();
// 	}

// 	PDEBUG("< expression");
// 	return left;
// }

// val_t parse_block(mc_parser_t *p) {
// 	PDEBUG("> block");
// 	ACCEPT(TOK_LBRACE);
// 	SKIP_NL();
// 	PARSE_STATEMENTS(stmts, TOK_RBRACE);
// 	ACCEPT(TOK_RBRACE);
// 	SKIP_NL();
// 	PDEBUG("< block");
// 	return stmts;
// }

// val_t parse_while(mc_parser_t *p) {
// 	PDEBUG("> while");
// 	ACCEPT(TOK_WHILE);
// 	PARSE(cond, expression, 0);
// 	SKIP_NL();
// 	PARSE(stmts, block);
// 	PDEBUG("< while");
// 	return MK2(while, cond, stmts);
// }

// val_t parse_if(mc_parser_t *p) {
// 	PDEBUG("> if");
// 	ACCEPT(TOK_IF);
// 	PARSE(cond, expression, 0);
// 	SKIP_NL();
// 	PARSE(stmts, block);
// 	PDEBUG("< if");
// 	return MK2(while, cond, stmts);
// }

// val_t parse_fn_def(mc_parser_t *p) {
// 	PDEBUG("> fn-def");
// 	ACCEPT(TOK_DEF);
// 	if (!AT(TOK_IDENT)) {
// 		ERROR("expected: identifier");
// 	}
// 	int name = rt_intern(p->lexer.tok, p->lexer.tok_len);
// 	NEXT();
// 	val_t params_head = mk_nil(), params_tail = mk_nil();
// 	if (AT(TOK_LPAREN)) {
// 		NEXT();
// 		if (!AT(TOK_RPAREN)) {
// 			while (1) {
// 				PARSE(param_name, ident);
// 				val_t node = mk_ast_list(param_name, mk_nil());
// 				if (nil_p(params_head)) {
// 					params_head = params_tail = node;
// 				} else {
// 					((ast_list_t*)params_tail.ast)->next = node;
// 					params_tail = node;
// 				}
// 				if (AT(TOK_COMMA)) {
// 					NEXT();
// 				} else {
// 					break;
// 				}
// 			}
// 		}
// 		ACCEPT(TOK_RPAREN);
// 	}
// 	SKIP_NL();
// 	PARSE(body, block);
// 	PDEBUG("< fn-def");
// 	return mk_ast_fn_def(name, params_head, body);
// }

// val_t parse_statement(mc_parser_t *p, int terminator) {
// 	PDEBUG("> statement");
// 	val_t stmt;
// 	if (AT(TOK_WHILE)) {
// 		PARSE_INTO(stmt, while);
// 	} else if (AT(TOK_IF)) {
// 		PARSE_INTO(stmt, if);
// 	} else if (AT(TOK_DEF)) {
// 		PARSE_INTO(stmt, fn_def);
// 	} else {
// 		PARSE_INTO(stmt, expression, 0);
// 		if (AT(TOK_NL)) {
// 			SKIP_NL();
// 		} else if (AT(terminator)) {
// 			// do nothing
// 		} else {
// 			ERROR("expected: newline or terminator");
// 		}
// 	}
// 	PDEBUG("< statement");
// 	return stmt;
// }

// val_t parse_statements(mc_parser_t *p, int terminator) {
// 	PDEBUG("> statements");
// 	val_t head = mk_nil(), tail = mk_nil();
// 	while (!AT(terminator)) {
// 		PARSE_STATEMENT(stmt, terminator);
// 		val_t node = mk_ast_list(stmt, mk_nil());
// 		if (nil_p(head)) {
// 			head = tail = node;
// 		} else {
// 			((ast_list_t*)tail.ast)->next = node;
// 			tail = node;
// 		}
// 	}
// 	PDEBUG("< statements");
// 	return head;
// }

// val_t parse_module(mc_parser_t *p) {
// 	PDEBUG("> module");
// 	SKIP_NL();
// 	PARSE_STATEMENTS(stmts, TOK_EOF);
// 	ACCEPT(TOK_EOF);
// 	PDEBUG("< module");
// 	return stmts;
// }

// /* Public Interface */



// val_t rt_parse_module(mc_parser_t *parser) {
// 	return parse_module(parser);
// }

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