#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        error("%s: invalid number of arguments", argv[0]);
    }

    token = tokenize(argv[1]);
    Node *node = parse();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    gen(node);
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}