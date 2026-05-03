#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>
#include <memory>
#include <string>
#include <vector>
#include "ast.h"
#include "token.h"
#include "environment.h" // 這裡現在會連帶引入 RuntimeError

// 實作 ExprVisitor 與 StmtVisitor
class Interpreter : public ExprVisitor, public StmtVisitor {
public:
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();

    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

    // --- ExprVisitor 實作 ---
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;

    // --- StmtVisitor 實作 ---
    void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
    void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
    void visitVarStmt(std::shared_ptr<VarStmt> stmt) override;
    void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;

    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);

private:
    std::any evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);
    
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);
    void checkNumberOperand(const Token& op, const std::any& operand);
    void checkNumberOperands(const Token& op, const std::any& left, const std::any& right);
    std::string stringify(const std::any& object);
};

#endif