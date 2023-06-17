#include "9cc.h"

int count()
{
    static int count = 0;
    ++count;
    return count;
}

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR)
        error("requrie lvalue");
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node)
{
    switch (node->kind)
    {
    case ND_EMPTY:
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_BLOCK:
        Node *cur = node->next;
        while (cur)
        {
            gen(cur);
            cur = cur->next;
            if (cur != NULL)
            {
                printf("  pop rax\n");
            }
        }
        return;
    case ND_IF:
        int label_number = count();
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L.else.%d\n", label_number);
        gen(node->then);
        printf("  jmp .L.end.%d\n", label_number);
        printf(".L.else.%d:\n", label_number);
        if (node->els)
            gen(node->els);
        printf(".L.end.%d:\n", label_number);
        return;
    case ND_WHILE: {
        int label_number = count();
        printf(".L.while.begin.%d:\n", label_number);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L.while.end.%d\n", label_number);
        gen(node->then);
        printf("  jmp .L.while.begin.%d\n", label_number);
        printf(".L.while.end.%d:\n", label_number);
        return;
    }
    case ND_FOR: {
        int label_number = count();
        gen(node->init);
        printf(".L.for.begin.%d:\n", label_number);
        if (node->cond->kind != ND_EMPTY)
        {
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .L.for.end.%d\n", label_number);
        }
        gen(node->then);
        gen(node->inc);
        printf("  jmp .L.for.begin.%d\n", label_number);
        printf(".L.for.end.%d:\n", label_number);
        return;
    }
    }

    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    switch (node->kind)
    {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NUM:
        error("unexpected logical error");
    }
    printf("  push rax\n");
}