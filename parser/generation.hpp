#ifndef JRUN_GENERATION_HPP
#define JRUN_GENERATION_HPP

#include <vector>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace jrun{
  namespace generation{
    using namespace std;    

    typedef std::string literValue;
    struct funCall; struct commands; struct AnnoFunc;
    typedef std::vector<std::string> names;
    struct mapKey {
      names map;
      names key;
    };
    struct mapConst {
      names map;
      literValue key;
    };
    typedef boost::variant<
      mapKey, mapConst,  names
    > leftValue;
    struct propertyAssign;
    struct Objectdef{
      std::vector<propertyAssign> properties;
    };
    struct dOpValue; struct sOpValue;
    typedef boost::variant<
     boost::recursive_wrapper<AnnoFunc>, 
     boost::recursive_wrapper<dOpValue>, boost::recursive_wrapper<sOpValue>, 
     boost::recursive_wrapper<funCall>, boost::recursive_wrapper<Objectdef>,
     leftValue, literValue 
    > rightValue;
    typedef boost::variant<
     boost::recursive_wrapper<funCall>,
     leftValue, literValue 
    > tValue;
    struct dOpValue { //exclude =
      tValue first;  std::string op;	tValue second;
    };
    struct sOpValue {
      tValue first; std::string op;
    };
    typedef std::vector<rightValue> RealArgs;
    struct funCall {
      leftValue name;
      RealArgs args;
    };   
    
    struct Assign; struct NamedFunc;
    typedef boost::variant<
      boost::recursive_wrapper<Assign>, rightValue
    > Expr;
    struct Assign {
      leftValue key;
      rightValue value;
    };    
    
    struct retCommand {
      rightValue exr;
    };
    
    typedef boost::variant<
      boost::recursive_wrapper<NamedFunc>, retCommand, Expr
    > mCommand;
    struct propertyAssign {
      std::string key;
      rightValue value;
    };     
    
    typedef std::vector<std::string> virtualArgs;
    struct AnnoFunc {
      virtualArgs argsv;
      std::vector<mCommand> commands;
    };
    struct NamedFunc {
      std::string name;
      virtualArgs argsv;
      std::vector<mCommand> commands;
    };       
    
    struct AST {
      std::vector<mCommand> commands;
    };
  }
}

#endif