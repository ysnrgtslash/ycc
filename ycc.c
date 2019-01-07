#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


enum {
    TK_NUM=256, // all other char is just char code
    TK_IDENT,
    TK_EOF,
  };

typedef struct {
    int ty;
    int val;
    char input;
  } Token;

enum {
    ND_NUM=256,
    ND_IDENT,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
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

Node *new_node_var(char input) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name=input;
    return node;
}

Node *term();
Node *add();
Node *mul();
void program();

void error(int i);


Token tokens[100];
Node *code[100];


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
     if(tokens[pos].ty == TK_IDENT)
        return new_node_var(tokens[pos++].input);

}


Node *mul() {
   Node *node =term();
    for (;;) {
        if (consume('*')) 
            node=new_node('*', node, term()); 
        else if (consume('/')) 
            node=new_node('/', node, term());
        else if (consume('=')) 
            node=new_node('=', node, term());
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

void program() {
    int codeline=0;
pos=0;
    while (tokens[pos].ty != TK_EOF) {
            code[codeline] = add();
            consume(';'); 
        codeline++;
    }
    code[codeline]= NULL;
}



void tokenize( char *p) {
       int i=0;
      while (*p) {
          if(isspace(*p)){
              p++;
              continue;
          }
          if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == ';' || *p == '=' ) {
              tokens[i].ty=*p; // char doce 
              tokens[i].input = *p;  // pointer of str
              p++;
              i++;
              continue;
          }

          if (isdigit(*p)){ 
              tokens[i].ty = TK_NUM;
              tokens[i].input= *p;
              tokens[i].val = strtol(p,&p,10);
              i++;
              continue;
          }
          if ('a' <= *p && *p <= 'z') {
              tokens[i].ty=TK_IDENT;
              tokens[i].input =*p;
              i++;
              p++;
                  continue;
          }

          fprintf(stderr, "can not tokenize:%s\n", p);
          exit(1);
      }
      tokens[i].ty = TK_EOF;
      tokens[i].input=*p;
  }

void error(int i) { fprintf(stderr, "can not tokenize:%s\n", tokens[i].input);
      exit(1);
 }

void gen_lval(Node *node) {
    if(node->ty != ND_IDENT) fprintf(stderr, "left value is not variable:%s\n");
    int offset = ('z' - node->name +1 ) *8 ; // offset from RBP
    printf("  mov rax,rbp\n");
    printf("  sub rax,%d\n", offset);
    printf("  push rax\n");
}

void gen(Node *node){
    if (node->ty==ND_NUM) {
        printf("  push %d\n",node->val);
        return;
    }
    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n"); // load value of rax address and put it to rax
        printf("  push rax\n");
        return;
    }

    if (node->ty== '=') {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rax\n");
        return;
        }

    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
       break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
     }  
    printf("  push rax\n");

}
 


int main(int argc, char **argv) { 
  if (argc != 2) {
    fprintf(stderr, "Args are no corret\n");
    return 1;
  }
  int i=0;
  tokenize(argv[1]);
  program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("  pop rax\n");
    }
    printf("  mov rsp,rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

  return 0;
}
