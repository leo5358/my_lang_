#include <iostream>
#include <cmath>
#include "interpreter.h"

// --- 進入點 ---
void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        for (const std::shared_ptr<Stmt>& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
    }
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    if (stmt != nullptr) {
        stmt->accept(*this);
    }
}

std::any Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
}

// --- StmtVisitor 實作 ---

void Interpreter::visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) {
    evaluate(stmt->expression);
}

void Interpreter::visitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
    std::any value = evaluate(stmt->expression);
    std::cout << stringify(value) << "\n";
}

void Interpreter::visitVarStmt(std::shared_ptr<VarStmt> stmt) {
    std::any value;
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }

    environment->define(stmt->name.lexeme, value);
}

void Interpreter::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) {
    // 建立新的區域環境，並將當前環境設為它的 enclosing (外層)
    executeBlock(stmt->statements, std::make_shared<Environment>(environment));
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> blockEnvironment) {
    std::shared_ptr<Environment> previous = this->environment;
    
    try {
        // 切換到新的作用域
        this->environment = blockEnvironment;

        for (const std::shared_ptr<Stmt>& statement : statements) {
            execute(statement);
        }
    } catch (...) {
        // 確保發生例外時，環境還是能切換回去
        this->environment = previous;
        throw;
    }
    
    // 執行完畢，恢復原本的作用域
    this->environment = previous;
}

// --- ExprVisitor 實作 ---

std::any Interpreter::visitVariableExpr(std::shared_ptr<Variable> expr) {
    return environment->get(expr->name);
}

std::any Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr) {
    std::any value = evaluate(expr->value);
    environment->assign(expr->name, value);
    return value;
}

std::any Interpreter::visitLiteralExpr(std::shared_ptr<Literal> expr) {
    return expr->value;
}

std::any Interpreter::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
    return evaluate(expr->expression);
}

std::any Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr) {
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            checkNumberOperand(expr->op, right);
            return -std::any_cast<double>(right);
        default:
            break;
    }

    return std::any{};
}

std::any Interpreter::visitBinaryExpr(std::shared_ptr<Binary> expr) {
    std::any left = evaluate(expr->left);
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
        case TokenType::MINUS:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::SLASH:
            checkNumberOperands(expr->op, left, right);
            if (std::any_cast<double>(right) == 0) {
                throw RuntimeError(expr->op, "Division by zero.");
            }
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::STAR:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
        case TokenType::PLUS:
            if (left.type() == typeid(double) && right.type() == typeid(double)) {
                return std::any_cast<double>(left) + std::any_cast<double>(right);
            }
            if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
            }
            throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");

        case TokenType::GREATER:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
            
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
            
        default:
            break;
    }

    return std::any{};
}

// --- 輔助方法實作 ---
// (與之前相同，故省略部分實作細節，請保留原本的內容)
bool Interpreter::isTruthy(const std::any& object) {
    if (!object.has_value()) return false;
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
    return true;
}

bool Interpreter::isEqual(const std::any& a, const std::any& b) {
    if (!a.has_value() && !b.has_value()) return true;
    if (!a.has_value()) return false;
    if (a.type() != b.type()) return false;

    if (a.type() == typeid(double)) return std::any_cast<double>(a) == std::any_cast<double>(b);
    if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    if (a.type() == typeid(bool)) return std::any_cast<bool>(a) == std::any_cast<bool>(b);

    return false;
}

void Interpreter::checkNumberOperand(const Token& op, const std::any& operand) {
    if (operand.type() == typeid(double)) return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& op, const std::any& left, const std::any& right) {
    if (left.type() == typeid(double) && right.type() == typeid(double)) return;
    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(const std::any& object) {
    if (!object.has_value()) return "nil";

    if (object.type() == typeid(double)) {
        std::string text = std::to_string(std::any_cast<double>(object));
        text.erase(text.find_last_not_of('0') + 1, std::string::npos);
        if (text.back() == '.') text += "0";
        return text;
    }

    if (object.type() == typeid(bool)) return std::any_cast<bool>(object) ? "true" : "false";
    if (object.type() == typeid(std::string)) return std::any_cast<std::string>(object);

    return "unknown";
}