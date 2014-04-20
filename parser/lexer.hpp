#ifndef JRUN_LEXER_HPP
#define JRUN_LEXER_HPP

#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_tokenize_and_parse.hpp>
#include <boost/spirit/include/lex_primitives.hpp>
#include <boost/spirit/include/lex_char_token_def.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

namespace jrun {
  namespace lexer{
    namespace lex = boost::spirit::lex;
    namespace ascii = boost::spirit::ascii;
    
    enum tokenID
    {
      IDANY = lex::min_token_id + 1,
      BRACE = IDANY + 1,
    };
    
    template<typename T>
    struct mLexer : lex::lexer<T>
    {
    public:
      mLexer();
      lex::token_def<> name;
      lex::token_def<> key_function, key_var, key_if, key_while;
      lex::token_def<> constStr;
      lex::token_def<> dOp, sOp, ret;
      lex::token_def<> opBracket, clBracket, opBrace, clBrace, opParen, clParen;
      lex::token_def<> dot, comma, semicolon, questionMark, colon;
      lex::token_def<> equal, less, more, negate;
      lex::token_def<> userchar, nchar;
    };
    
    template<typename T>
    mLexer<T>::mLexer() : 
      name("[a-zA-Z]+[a-zA-Z0-9_]*"),
      key_function("function"), key_var("var"), key_if("if"), key_while("while"),
      constStr("(\\\"[^\n\"]*\\\")|(\\d)+"),
      dOp("[\\+\\-\\*]"), sOp("\\+\\+|\\-\\-"),
      opBracket("\\["), clBracket("\\]"), opBrace("\\{"), clBrace("\\}"), opParen("\\("), clParen("\\)"),
      dot("\\."), comma(","), semicolon(";"), questionMark("\\?"), colon(":"),
      equal("="), less("<"), more(">"), negate("-"),
      userchar("[a-zA-Z_1-9]"), nchar("[^a-zA-Z_1-9]"), ret("return")
      {
	this->self( "WS" ) = lex::token_def<>( "[ \\t\\n]+" );
	this->self.add
	  (name)
	  (key_function) (key_var) (key_if) (key_while)
	  (constStr) (dOp) (sOp)
	  (opBracket) (clBracket) (opBrace) (clBrace) (opParen) (clParen)
	  (dot) (comma) (semicolon) (questionMark) (colon)
	  (equal) (less) (more) (negate)
	;	
      };
  }
}

#endif