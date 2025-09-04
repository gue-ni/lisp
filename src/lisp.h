#pragma once

enum ExprType {
    NIL,
    SYMBOL,
    NUMBER,
    STRING,
    BOOLEAN,
    LIST,
};

struct Expr {
    ExprType type;
    union {
        ExprSymbol symbol;
        ExprNumber number;
        ExprString string;
        ExprBoolean boolean;
        ExprList list;
    } as;
};

struct Environment {
    // 
};

int eval(const char*);

int eval(const Expr*, Environment*);

Expr* parse(const char*, Environment*);

int repl();