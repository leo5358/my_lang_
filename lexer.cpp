#include <iostream>
#include <cctype> // isdigit(), isalpha(), isalnum()
#include "lexer.h"

using namespace std;

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"and",    TokenType::AND},
    {"class",  TokenType::CLASS},
    {"else",   TokenType::ELSE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"fun",    TokenType::FUN},
    {"if",     TokenType::IF},
    {"nil",    TokenType::NIL},
    {"or",     TokenType::OR},
    {"print",  TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super",  TokenType::SUPER},
    {"this",   TokenType::THIS},
    {"true",   TokenType::TRUE},
    {"var",    TokenType::VAR},
    {"while",  TokenType::WHILE}
};

bool Lexer::isEnd(){
    return current >= source.length();
}

char Lexer::advance(){
    return source[current++];
}

char Lexer::peek(){
    if(isEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext(){
    if(current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected){
    if(isEnd()) return false;
    if(source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::string(){
    while(peek() != '"' && !isEnd()){
        if(peek() == '\n') line++;
        advance();
    }
    if(isEnd()){
        cerr << "Unterminated string.\n";
        return;
    }
    advance(); // swallow "
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}


void Lexer::addToken(TokenType type) {
    addToken(type, std::any{});
}

void Lexer::addToken(TokenType type, std::any literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance(); 
        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::IDENTIFIER;
    }
    
    addToken(type);
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        // 單一字元
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break; 
        // 包含一或兩個字元的運算子
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        // 斜線或註解
        case '/':
            if (match('/')) {
                // 註解會持續到行尾
                while (peek() != '\n' && !isEnd()) advance();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        // 忽略空白字元
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        // 字串字面值
        case '"': string(); break;
        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                cerr << "Unexpected character at line " << line << ".\n";
            }
            break;
    }
}