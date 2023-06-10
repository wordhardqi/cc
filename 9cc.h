
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  utils
bool startswith(char *p, char *q);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// Parser
typedef enum
{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token
{
    TokenKind kind;
    struct Token *next;
    int val;
    char *str;
    int len;
} Token;

extern char *uesr_input;
extern Token *token;

typedef enum
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LT,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Token *tokenize(char *p);
Node *parse();

// codegen

void gen(Node *tree);