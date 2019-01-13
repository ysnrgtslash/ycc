#ifndef YCC_H
#define YCC_H

enum {
    TK_NUM=256, // all other char is just char code
    TK_IDENT,
    TK_EOF,
  };

typedef struct {
    int ty;
    int val;
    char input;
  } Token ;

enum {
    ND_NUM=256,
    ND_IDENT,
    ND_EQ,
    ND_NONEQ,
    ND_ERR,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

void gen_lval(Node *node); 
void gen(Node *node);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_var(char input);
Node *term();
Node *add();
Node *mul();
void program();
void error(int i);
int consume(int ty);
void tokenize( char *p);
void error(int i);

#endif // YCC_H
