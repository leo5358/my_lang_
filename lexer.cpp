#include<iostream>
#include<cctype> //isdigit(), isalpha(), isalnum()
#include "lexer.h"

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
    {"while",  TokenType::WHILE},
}

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
    cerr << "Unterminated string. ";
    return ;
  }
  
  advance(); //swallow "
  
  std::string value = source.substr(start + 1, current - start - 2);
  addToken(TokenType::STRING, value);

}
