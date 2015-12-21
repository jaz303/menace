enum {
	AST_LIST,
	AST_ASSIGN,
	AST_SEND,
	AST_UNARY_MSG,
	AST_MSG_PART
};

enum {
	T_NIL,
	T_AST,
	T_INT,
	T_STRING,
	T_IDENT,
	T_TRUE,
	T_FALSE
};

typedef struct {
    int length;
    char str[0];
} mc_string_t;

typedef struct val {
	int type;
	union {
		ast_node_t *ast;
		int32_t ival;
		mc_string_t *str;
	};
} val_t;
