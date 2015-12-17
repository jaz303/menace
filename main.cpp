#include <stdio.h>
#include <stdlib.h>

typedef struct ast_node ast_node_t;

#include "util.inc.cpp"
#include "types.inc.cpp"
#include "val.inc.cpp"
#include "ast.inc.cpp"
#include "lexer.inc.cpp"

int main(int argc, char *argv[]) {
	char linebuffer[512];

	while (!feof(stdin)) {
		putc('>', stdout);
		putc(' ', stdout);
		if (fgets(linebuffer, sizeof(linebuffer), stdin) == NULL) {
			break;
		}
	}

	printf("Bye!\n");

	return 0;
}