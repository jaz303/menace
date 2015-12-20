#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ast_node ast_node_t;

#include "util.inc.cpp"
#include "intern.inc.cpp"
#include "types.inc.cpp"
#include "val.inc.cpp"
#include "ast.inc.cpp"
#include "lexer.inc.cpp"
#include "parser.inc.cpp"

int main(int argc, char *argv[]) {
	mc_intern_init();

	char linebuffer[512];

	while (!feof(stdin)) {
		putc('>', stdout);
		putc(' ', stdout);
		if (fgets(linebuffer, sizeof(linebuffer), stdin) == NULL) {
			break;
		}

		mc_parser_t parser;
		mc_lexer_init(&parser.lexer, linebuffer);
		mc_parser_init(&parser);

		val_t ast = mc_parse_repl_line(&parser);
		if (nil_p(ast)) {
			printf("parse error\n");
		} else {
			// TODO: do stuff
		}
	}

	printf("Bye!\n");

	return 0;
}