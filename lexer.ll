%{
#include <string>

#include "scanner.h"

using Token = Riser::Parser::token;

#define yyterminate() return( Token::YYEOF )

#define YY_USER_ACTION yylloc->step(); yylloc->columns(yyleng);
#define YY_VERBOSE

%}

%option c++
%option	noyywrap
%option warn
%option outfile="scanner.cc"

%{

int yyFlexLexer::yylex() { abort(); }

%}


INTEGER               [0-9]+
ID                  [a-z][a-z0-9]*
FORMAT_CHARS        [\t\r\n ]+

%%

"+"         return Token::PLUS;
"-"         return Token::MINUS;
"*"         return Token::STAR;
"/"         return Token::SLASH;
"="         return Token::EQUAL;
"("         return Token::LPAREN;
")"         return Token::RPAREN;
";"         return Token::SEMICOLON;

"int"            return Token::INT;
"string"         return Token::STRING;

"print"         return Token::PRINT;
"if"            return Token::IF;
"else if"       return Token::ELSE_IF;
"else"          return Token::ELSE;
"while"         return Token::WHILE;
"for"           return Token::FOR;

{INTEGER}       yylval->emplace<int>(atoi(yytext)); return Token::NUM;

{ID}            yylval->emplace<std::string>(yytext); return Token::ID;

{FORMAT_CHARS}         ;  

<<EOF>>					return Token::YYEOF;

%%