struct ast_node {
    int type;
};

typedef struct ast_list ast_list_t;
struct ast_list {
    ast_node_t base;
    val_t exp, next;
};

typedef struct ast_assign {
    ast_node_t base;
    val_t assignee, exp;
} ast_assign_t;

typedef struct ast_send {
    ast_node_t base;
    val_t receiver, message;
} ast_send_t;

typedef struct ast_unary_msg {
    ast_node_t base;
    val_t selector;
} ast_unary_msg_t;

typedef struct ast_msg_part {
    ast_node_t base;
    val_t name, arg;
} ast_msg_part_t;

#define ALLOC_AST(struct_type, tag) \
    struct_type *node = (struct_type*)malloc(sizeof(struct_type)); \
    ((ast_node_t*)node)->type = tag; \
    val_t val = { .type = T_AST, .ast = (ast_node_t*) node }

val_t mk_ast_list(val_t stmt, val_t next) {
    ALLOC_AST(ast_list_t, AST_LIST);
    node->exp = stmt;
    node->next = next;
    return val;
}

val_t mk_ast_assign(val_t assignee, val_t exp) {
    ALLOC_AST(ast_assign_t, AST_ASSIGN);
    node->assignee = assignee;
    node->exp = exp;
    return val;
}

val_t mk_ast_send(val_t receiver, val_t message) {
    ALLOC_AST(ast_send_t, AST_SEND);
    node->receiver = receiver;
    node->message = message;
    return val;
}

val_t mk_ast_unary_msg(val_t selector) {
    ALLOC_AST(ast_unary_msg_t, AST_UNARY_MSG);
    node->selector = selector;
    return val;   
}

val_t mk_ast_msg_part(val_t name, val_t arg) {
    ALLOC_AST(ast_msg_part_t, AST_MSG_PART);
    node->name = name;
    node->arg = arg;
    return val;   
}

int ast_type(val_t v) {
    return v.ast->type;
}

int ast_list_len(val_t v) {
    int len = 0;
    while (!nil_p(v)) {
        len++;
        v = ((ast_list_t*)v.ast)->next;
    }
    return len;
}