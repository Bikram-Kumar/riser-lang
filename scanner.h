#pragma once

#ifndef __FLEX_LEXER_H
#include "FlexLexer.h"
#endif

#include "parser.hh"  
#include "scanner.h"  
#define YY_DECL \
    Riser::Parser::token_type Riser::Scanner::lex( \
            Riser::Parser::semantic_type* yylval, Riser::Parser::location_type* yylloc \
    )


namespace Riser
{

class Scanner : public yyFlexLexer
{
public:
    Scanner() {}

    virtual Parser::token_type lex(Parser::semantic_type* yylval, Parser::location_type* yylloc);
};

}