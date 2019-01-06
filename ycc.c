#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


enum {
    TK_NUM=256, // all other char is just char code
    TK_EOF,
  };

typedef struct {
    int ty;
    int val;
    char *input;
  } Token;

enum {
    ND_NUM=256,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs){
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val=val;
    return node;
}

Node *term();
Node *add();
Node *mul();
void error(int i);


Token tokens[100];
int pos=0;

int consume(int ty){
    if (tokens[pos].ty != ty )
        return 0;
    pos++;
    return 1;
}

Node *term() {
    if (consume('(')) {
            Node *node = add();
            if(!consume(')'))
                error(tokens[pos].ty);
            return node; 
                }
     if(tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);

}


Node *mul() {
   Node *node =term();
    for (;;) {
        if (consume('*')) 
            node=new_node('*', node, term()); 
        else if (consume('/')) 
            node=new_node('/', node, term());
        else
            return node;
            }
   return node;
}
	
Node *add() { 
    Node *node = mul();
    for (;;) {
        if (consume('+')) 
            node=new_node('+', node, mul()); 
        else if (consume('-')) 
            node=new_node('-', node, mul());
        else
            return node;
            }
}

void tokenize( char *p) {
       int i=0;
      while (*p) {
          if(isspace(*p)){
              p++;
              continue;
          }
          if (*p == '+' || *p == '-') {
              tokens[i].ty=*p; // char doce 
              tokens[i].input = p;  // pointer of str
              p++;
              i++;
              continue;
          }

          if (isdigit(*p)){ 
              tokens[i].ty = TK_NUM;
              tokens[i].input=p;
              tokens[i].val = strtol(p,&p,10);
              i++;
              continue;
          }

          fprintf(stderr, "can not tokenize:%s\n", p);
          exit(1);
      }
      tokens[i].ty = TK_EOF;
      tokens[i].input=p;
  }

void error(int i) { fprintf(stderr, "can not tokenize:%s\n", tokens[i].input);
      exit(1);
 }

int main(int argc, char **argv) { 
  if (argc != 2) {
    fprintf(stderr, "Args are no corret\n");
    return 1;
  }
  int i=0;
  tokenize(argv[1]);
  Node *node=add();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    if (tokens[0].ty != TK_NUM ) error(0);
    printf("  mov rax, %d\n", tokens[0].val);
    i++;
    while (tokens[i].ty != TK_EOF) {

    if ( tokens[i].ty == '+') {
        i++;
        if (tokens[i].ty != TK_NUM) error(i);
        printf("  add rax, %ld\n",tokens[i].val);
        i++;
        continue;
    }

    if ( tokens[i].ty == '-') {
        i++;
        if (tokens[i].ty != TK_NUM) error(i);
        printf("  sub rax, %ld\n",tokens[i].val);
        i++;
        continue;
    }
    error(i);
    }


  printf("  ret\n");
  return 0;
}
