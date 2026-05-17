#ifndef LEXER_H
#define LEXER_H

#include<vector>
#include<string>
#include<unordered_map>

#include "token.h"

class Lexer{
private:
  const std::string source;
  std::vector<Token>tokens;

  //pointers (cursor)
  size_t start = 0;
  size_t current = 0;
  
  //current line 
  int line = 1;


public: 
  Lexer(std::string source) : source(source) {}

  //scan and return all token
  std::vector<Token> scanTokens(){
    while(!isEnd()){
      start = current;
      scanToken();
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, "", std::any{}, line);
    return tokens;
  }

private:
  //keywords dict (identify identifier is variable name or keywords)
  static const std::unordered_map<std::string, TokenType>keywords;
  
  void scanToken();

  //deal with types
  void string();
  void number();
  void identifier();

  bool isEnd();
  char advance();
  bool match(char expected);
  char peek();
  char peekNext();

  void addToken(TokenType type);
  void addToken(TokenType type, std::any literal);
};


#endif


