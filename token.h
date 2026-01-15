#ifndef TOKEN_H
#define TOKEN_H

#include<string>
#include<any>
#include "token_type.h"

class Token{
public: 
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  Token(TokenType type, std::string lexeme, std::any literal, int line)
  : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) 
  {}

  std::string toString(){
    return std::to_string((int)type) + " " + lexeme + " " + std::tp_string(line);
  }
}

#endif
