enum {
	AST_LIST,
	AST_ASSIGN
};

enum {
	T_NIL,
	T_AST,
	T_INT,
	T_STRING,
	T_IDENT
};

typedef struct val {
	int type;
	union {
		ast_node_t *ast;
	};
} val_t;