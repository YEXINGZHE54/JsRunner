#include "parser/interface.hpp"
#include "parser.hpp"
#include <boost/spirit/include/qi.hpp>

using namespace jrun::parser;
namespace ascii = boost::spirit::ascii;

    template <typename Iter>
    bool parseT(Iter& begin, Iter& end, genPtr mPtr) {      
      mXml_grammer< Iter > mGrammar;

      bool r = qi::phrase_parse(begin, end, mGrammar, boost::spirit::ascii::space, *mPtr);
      if(r) std::cout << "partial matched!" << std::endl;

      if(begin != end) return false;
      return r;
    }
   
bool jrun::parser::parse(char const* begin, char const * end, genPtr mPtr)
{
  return parseT(begin, end, mPtr);
}