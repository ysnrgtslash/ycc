#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "ycc.h"

void gen_lval(Node *node) {
    if(node->ty != ND_IDENT) fprintf(stderr, "left value is not variable:\n");
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
        break;
    case ND_EQ:
        printf("  cmp rdi, rax\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NONEQ:
        printf("  cmp rdi, rax\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
     }  
    printf("  push rax\n");

}
 
