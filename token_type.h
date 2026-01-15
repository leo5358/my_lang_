#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum class TokenType{
  //single char symbol
  LEFT_PAREN, RIGHT_PAREN, LBRACE, RBRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
  
  //one or two char symbol(bang -> !)
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  //Literals
  IDENTIFIER, STRING, NUMBER,

  //keywords
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  
  //EOF
  EOF_TOKEN
};

#endif
