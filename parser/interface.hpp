#ifndef JRUN_PARSER_INTERFACE_HPP
#define JRUN_PARSER_INTERFACE_HPP

#include <memory>
#include <boost/fusion/include/adapt_struct.hpp>
#include <vector>
#include "generation.hpp"

namespace jrun{
  namespace parser{
    namespace data = jrun::generation;
    
    typedef data::AST genType;
    typedef std::shared_ptr<genType> genPtr;    
    
    bool parse(const char * begin, const char * end, genPtr mPtr);
  }
}

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::mapKey,
  (jrun::generation::names, map)
  (jrun::generation::names, key)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::mapConst,
  (jrun::generation::names, map)
  (jrun::generation::literValue, key)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::dOpValue,
  (jrun::generation::wrappedRightValue, first)
  (std::string, op)
  (jrun::generation::rightValue, second)
  (bool, grouped)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::sOpValue,
  (jrun::generation::tValue, first)
  (std::string, op)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::funCall,
  (jrun::generation::leftValue, name)
  (jrun::generation::RealArgs, args)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::Assign,
  (jrun::generation::leftValue, key)
  (jrun::generation::rightValue, value)
)

BOOST_FUSION_ADAPT_STRUCT(
      jrun::generation::Objectdef,
      (std::vector<jrun::generation::propertyAssign>, properties)
)

BOOST_FUSION_ADAPT_STRUCT(
    jrun::generation::propertyAssign,
    (std::string, key)
    (jrun::generation::rightValue, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    jrun::generation::retCommand,
    (jrun::generation::rightValue, exr)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::AnnoFunc,
  (jrun::generation::virtualArgs, argsv)
  (std::vector<jrun::generation::mCommand>, commands)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::NamedFunc,
  (std::string, name)
  (jrun::generation::virtualArgs, argsv)
  (std::vector<jrun::generation::mCommand>, commands)
)

BOOST_FUSION_ADAPT_STRUCT(
      jrun::generation::ifCommand,
      (jrun::generation::Expr, e)
      (std::vector<jrun::generation::mCommand>, commands)
      (std::vector<jrun::generation::mCommand>, elsecoms)
)

BOOST_FUSION_ADAPT_STRUCT(
      jrun::generation::forCommand,
      (std::vector<jrun::generation::Expr>, start)
      (jrun::generation::Expr, condition)
      (std::vector<jrun::generation::Expr>, after)
      (std::vector<jrun::generation::mCommand>, commands)
)

BOOST_FUSION_ADAPT_STRUCT(
      jrun::generation::whileCommand,
      (jrun::generation::Expr, condition)
      (std::vector<jrun::generation::mCommand>, commands)
)

BOOST_FUSION_ADAPT_STRUCT(
  jrun::generation::AST,
  (std::vector<jrun::generation::mCommand>, commands)
)

#endif