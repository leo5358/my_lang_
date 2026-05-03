#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

class Lox {
    static Interpreter interpreter;
    static bool hadError;
    static bool hadRuntimeError;

public:
    static void runFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Could not open file " << path << std::endl;
            exit(74);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        run(buffer.str());

        if (hadError) exit(65);
        if (hadRuntimeError) exit(70);
    }

    static void runPrompt() {
        std::string line;
        for (;;) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            run(line);
            hadError = false; // 互動模式下，單行錯誤不應結束程式
        }
    }

private:
static void run(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        // 修改這裡：接收多個 statements
        std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

        // 如果解析過程有錯，停止執行
        if (hadError) return;

        // 執行所有陳述式
        interpreter.interpret(statements);
    }

public:
    static void error(int line, const std::string& message) {
        report(line, "", message);
    }

    static void runtimeError(const RuntimeError& error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
        hadRuntimeError = true;
    }

private:
    static void report(int line, const std::string& where, const std::string& message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
        hadError = true;
    }
};

// 靜態成員初始化
Interpreter Lox::interpreter;
bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: cpplox [script]" << std::endl;
        exit(64);
    } else if (argc == 2) {
        Lox::runFile(argv[1]);
    } else {
        Lox::runPrompt();
    }
    return 0;
}