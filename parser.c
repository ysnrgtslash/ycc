#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "ycc.h"


int pos=0;
Token tokens[100];
Node *code[100];

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
     error(pos); 
     exit(10);

}


Node *mul() {
   Node *node =term();
    for (;;) {
        if (consume('*')) 
            node=new_node('*', node, term()); 
        else if (consume('/')) 
            node=new_node('/', node, term());
        else if (consume('=')) {
            if(consume('=')) {
                node=new_node(ND_EQ,node,add());
            } else {
                node=new_node('=', node, add());
                }
            }
        else if (consume('!')) {
            if(consume('=')) {
                node=new_node(ND_NONEQ,node,add());
            } else {
                error(100);
            }
        }
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
          if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == ';' || *p == '=' || *p == '!' ) {
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

void error(int i) { fprintf(stderr, "can not tokenize:%c\n", tokens[i].input);
      exit(1);
 }

