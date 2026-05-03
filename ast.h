#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <any>
#include "token.h"

// 前置宣告 Expr 節點
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;

// 前置宣告 Stmt 節點
class ExpressionStmt;
class PrintStmt;
class VarStmt;
class BlockStmt;

// ==========================================
// ExprVisitor 介面 (處理有回傳值的運算式)
// ==========================================
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;
    virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
};

// ==========================================
// StmtVisitor 介面 (處理沒有回傳值的陳述式)
// ==========================================
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) = 0;
    virtual void visitVarStmt(std::shared_ptr<VarStmt> stmt) = 0;
    virtual void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
};

// ==========================================
// 運算式 (Expressions)
// ==========================================
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

class Binary : public Expr, public std::enable_shared_from_this<Binary> {
public:
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }
};

class Grouping : public Expr, public std::enable_shared_from_this<Grouping> {
public:
    std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }
};

class Literal : public Expr, public std::enable_shared_from_this<Literal> {
public:
    std::any value;

    Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }
};

class Unary : public Expr, public std::enable_shared_from_this<Unary> {
public:
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }
};

// 新增：讀取變數 (例如 a)
class Variable : public Expr, public std::enable_shared_from_this<Variable> {
public:
    Token name;

    Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }
};

// 新增：變數賦值 (例如 a = 1)
class Assign : public Expr, public std::enable_shared_from_this<Assign> {
public:
    Token name;
    std::shared_ptr<Expr> value;

    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }
};

// ==========================================
// 陳述式 (Statements)
// ==========================================
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

// 單純運算式，例如函式呼叫或運算後不接值的狀況
class ExpressionStmt : public Stmt, public std::enable_shared_from_this<ExpressionStmt> {
public:
    std::shared_ptr<Expr> expression;

    ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(shared_from_this());
    }
};

// 列印陳述式
class PrintStmt : public Stmt, public std::enable_shared_from_this<PrintStmt> {
public:
    std::shared_ptr<Expr> expression;

    PrintStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(shared_from_this());
    }
};

// 變數宣告 (例如 var a = 1;)
class VarStmt : public Stmt, public std::enable_shared_from_this<VarStmt> {
public:
    Token name;
    std::shared_ptr<Expr> initializer; // 可以為 nullptr (如 var a;)

    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(shared_from_this());
    }
};

// 區塊陳述式 (支援區域變數作用域)
class BlockStmt : public Stmt, public std::enable_shared_from_this<BlockStmt> {
public:
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitBlockStmt(shared_from_this());
    }
};

#endif