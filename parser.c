#include "9cc.h"
#include <string.h>
#include <stdlib.h>
bool startswith(char *p, char *q)
{
    return memcmp(p, q, strlen(q)) == 0;
}

bool startswithn(char *p, char *q, int len)
{
    return memcmp(p, q, len) == 0;
}

bool is_ident1(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}
bool is_alnum(char c)
{
    return is_ident1(c) || ('0' <= c && c <= '9');
}


char *uesr_input;
Token *token;

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - uesr_input;
    fprintf(stderr, "%s\n", uesr_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || !startswith(token->str, op))
        return false;
    token = token->next;
    return true;
}

Token *consume_by_kind(TokenKind kind)
{
    if (token->kind == kind)
    {
        Token *ret = token;
        token = token->next;
        return ret;
    }
    return NULL;
}

char *token_kind_to_str(TokenKind kind)
{
    switch (kind)
    {
    case TK_ELSE:
        return "else";
    case TK_IF:
        return "if";
    default:
        return "unknown token";
    }
}

void expect_by_kind(TokenKind kind)
{
    if (token->kind == kind)
    {
        Token *ret = token;
        token = token->next;
        return;
    }
    error_at(token->str, "expected %s", token_kind_to_str(kind));
}

void expect(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || !startswith(token->str, op))
        error_at(token->str, "expected '%s'", op);
    token = token->next;
}

bool peek(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || !startswith(token->str, op))
        return false;
    return true;
}

int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }
        if (startswith(p, ">=") || startswith(p, "<=") || startswith(p, "!=") || startswith(p, "=="))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr(",+-*/()><;={}&", *p))
        {
            cur = new_token(TK_RESERVED, cur, p, 1);
            p += 1;
            continue;
        }

        static char *keyword_strs[] = {"return", "if", "else", "while", "for"};
        static TokenKind keyword_Kinds[] = {TK_RETURN, TK_IF, TK_ELSE, TK_WHILE, TK_FOR};
        bool keyword_found = false;
        for (int i = 0; i < sizeof(keyword_strs) / sizeof(keyword_strs[0]); i++)
        {
            char *keyword = keyword_strs[i];
            TokenKind kind = keyword_Kinds[i];
            int len = strlen(keyword);
            if (strncmp(p, keyword, len) == 0 && !is_alnum(p[len]))
            {
                cur = new_token(kind, cur, p, len);
                p += len;
                keyword_found = true;
                break;
            }
        }
        if (keyword_found)
            continue;

        // if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
        // {
        //     cur = new_token(TK_RETURN, cur, p, 6);
        //     p += 6;
        //     continue;
        // }

        if (is_ident1(*p))
        {
            char *start = p;
            do
            {
                p++;
            } while (is_alnum(*p));

            cur = new_token(TK_IDENT, cur, start, p - start);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        error_at(p, "invalid token");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_var(Token *tok)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = tok->offset;
    return node;
}

// program    = stmt*
// stmt       = expr ";"
// | "if" "(" expr ")" stmt ("else" stmt)?
// | "while" "(" expr ")" stmt
// | "for" "(" expr? ";" expr? ";" expr? ")" stmt
// | "{" stmt* "}" | "return" expr ";"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary | "*" unary | "&" unary
// primary    = num | ident ("(" fun_args")")? | "(" expr ")"
// fun_args   = e | assign(, assign)?
Lvar *locals = NULL;

Lvar *find_lvar(Token *tok)
{
    for (Lvar *var = locals; var; var = var->next)
    {
        if (var->len = tok->len && startswithn(var->name, tok->str, tok->len))
        {
            return var;
        }
    }
    return NULL;
}

Lvar *new_lvar(Token *tok)
{
    Lvar *lvar = calloc(1, sizeof(Lvar));
}

Node *stmt();
Node *block_stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *funcall();
Node *parse()
{
    program();
    return NULL;
}

Node *code[100];
void program()
{
    int i = 0;
    while (!at_eof())
    {
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt()
{
    Node *node;

    if (consume("{"))
    {
        node = block_stmt();
        expect("}");
        return node;
    }

    if (consume_by_kind(TK_RETURN))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    }

    if (consume_by_kind(TK_IF))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;

        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume_by_kind(TK_ELSE))
        {
            node->els = stmt();
        }
        return node;
    }
    if (consume_by_kind(TK_WHILE))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }
    if (consume_by_kind(TK_FOR))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        node->init = expr();
        expect(";");
        node->cond = expr();
        expect(";");
        if (!peek(")"))
        {
            node->inc = expr();
        }
        else
        {
            Node *empty = calloc(1, sizeof(Node));
            empty->kind = ND_EMPTY;
            node->inc = empty;
        }
        expect(")");
        node->then = stmt();
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

Node *block_stmt()
{
    Node head;
    Node *cur_node = &head;
    while (!peek("}"))
    {
        cur_node->next = stmt();
        cur_node = cur_node->next;
    }

    Node *node = new_node(ND_BLOCK, NULL, NULL);
    node->next = head.next;

    return node;
}

Node *expr()
{
    if (peek(";"))
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_EMPTY;
        return node;
    }
    return assign();
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
    {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (consume("=="))
        {
            node = new_node(ND_EQ, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_NE, node, relational());
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();
    for (;;)
    {
        if (consume("<="))
        {
            node = new_node(ND_LE, node, add());
        }
        else if (consume("<"))
        {
            node = new_node(ND_LT, node, add());
        }
        else if (consume(">="))
        {
            node = new_node(ND_LE, add(), node);
        }
        else if (consume(">"))
        {
            node = new_node(ND_LT, add(), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();
    for (;;)
    {
        if (consume("+"))
        {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, node, mul());
        }
        else
        {
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();
    for (;;)
    {
        if (consume("*"))
        {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, node, unary());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume("+"))
    {
        return primary();
    }
    if (consume("-"))
    {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    if(consume("&"))
    {
        return new_node(ND_ADDR, unary(), NULL);
    }
    if(consume("*"))
    {
        return new_node(ND_DEREF,unary(), NULL);
    }
    return primary();
}

Node *primary()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *cur_token;
    Node *node;

    // ident() or ident
    if (cur_token = consume_by_kind(TK_IDENT))
    {
        if (peek("("))
        {
            Token *tk_func_name = cur_token;
            node = funcall(tk_func_name);
        }
        else
        {
            node = calloc(1, sizeof(Node));
            node->kind = ND_LVAR;

            Lvar *lvar = find_lvar(cur_token);
            if (lvar)
            {
                node->offset = lvar->offset;
            }
            else
            {
                lvar = calloc(1, sizeof(Lvar));
                lvar->next = locals;
                lvar->name = cur_token->str;
                lvar->len = cur_token->len;
                lvar->offset = locals == NULL ? 8 : locals->offset + 8;
                node->offset = lvar->offset;
                locals = lvar;
            }
        }

        return node;
    }

    if (cur_token = consume_by_kind(TK_NUM))
    {
        node = new_node_num(cur_token->val);
        return node;
    }
    error_at(token->str, "expected expression | ident | num");
    return NULL;
}
Node *funcall(Token* tk_func_name)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCALL;
    node->funcname = strndup(tk_func_name->str, tk_func_name->len);
    expect("(");
    if (consume(")"))
    {
        node->args = NULL;
    }
    else
    {
        Node *cur_node;
        node->args = assign();
        cur_node = node->args;
        while (!consume(")"))
        {
            expect(",");
            cur_node->next = assign();
            cur_node = cur_node->next;
        }
    }
    return node;
}