#pragma once
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  utils
bool startswith(char *p, char *q);
bool startswithn(char *p, char *q, int len);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// Parser

typedef enum
{
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_EOF,
} TokenKind;


typedef struct Token
{
    TokenKind kind;
    struct Token *next;
    int val;
    char *str;
    int len;
    int offset;
} Token;

extern char *uesr_input;
extern Token *token;

typedef enum
{
    ND_EMPTY,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LT,
    ND_ASSIGN,
    ND_LVAR,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCALL,
    ND_NUM,
} NodeKind;

typedef struct Lvar Lvar;
struct Lvar
{
    Lvar *next;
    char *name;
    int len;
    int offset;
};

extern Lvar *locals;


typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node * next;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;

    Node * cond;
    Node * then;
    Node * els; 
    Node * init;
    Node * inc;

    char * funcname;
    Node * args;
};

Token *tokenize(char *p);
Node *parse();

extern Node *code[100];
void program();

// codegen

void gen(Node *tree);