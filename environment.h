#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <any>
#include <memory>
#include <stdexcept>
#include "token.h"

// 將 RuntimeError 的定義移至此處，讓 Environment 可以使用它
class RuntimeError : public std::runtime_error {
public:
    const Token token;
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

class Environment : public std::enable_shared_from_this<Environment> {
private:
    std::unordered_map<std::string, std::any> values;
    std::shared_ptr<Environment> enclosing;

public:
    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    void define(const std::string& name, std::any value) {
        values[name] = value;
    }

    std::any get(const Token& name) {
        auto it = values.find(name.lexeme);
        if (it != values.end()) {
            return it->second;
        }

        if (enclosing != nullptr) {
            return enclosing->get(name);
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assign(const Token& name, std::any value) {
        auto it = values.find(name.lexeme);
        if (it != values.end()) {
            it->second = value;
            return;
        }

        if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
};

#endif