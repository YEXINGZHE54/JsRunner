#include <vmachine/calculator.hpp>
#include <vmachine/visitor.hpp>
#include <map>
#include <string>
#include <functional>
#include "vmachine/JObject.hpp"
#include <memory>
#include <boost/variant/get.hpp>
#include <vector>
#include <stack>
#include "vmachine/tag.hpp"
#include "vmachine/exception.hpp"
#include "log/logger.hpp"

using namespace jrun::vmachine;
namespace data = jrun::generation;

namespace jrun{
  namespace vmachine {
    namespace calculator{
      typedef std::function< JLiterObjectPtr(const JLiterObjectPtr, const JLiterObjectPtr) > functor;
      typedef std::map<const std::string,  functor> functorMap;
      static  functorMap functors;
      class functorsInit{
	public:
	functorsInit(functorMap& map){
	  map["+"] = [](const JLiterObjectPtr l1, const JLiterObjectPtr l2)->JLiterObjectPtr{ return l1->operator+(l2); };
	  map["-"] = [](const JLiterObjectPtr l1, const JLiterObjectPtr l2)->JLiterObjectPtr{ return l1->operator-(l2); };
	  map["*"] = [](const JLiterObjectPtr l1, const JLiterObjectPtr l2)->JLiterObjectPtr{ return l1->operator*(l2); };
	  map["/"] = [](const JLiterObjectPtr l1, const JLiterObjectPtr l2)->JLiterObjectPtr{ return l1->operator/(l2); };
	}
      };
      static functorsInit fInitializer(functors);
      typedef std::map<const std::string,  int> priorMap;
      static priorMap priorities;
      class priorInit{
      public:
	priorInit(priorMap& map){
	  map["+"] = 3;
	  map["-"] = 3;
	  map["*"] = 4;
	  map["/"] = 4;	  
	  map["start"] = 0;
	}
      };
      static priorInit pInitializer(priorities);
      typedef std::map<std::string, JLiterObjectPtr> operatorMap;
      static operatorMap operators;
      class operatorsInit
      {
      public:
	operatorsInit(operatorMap& map)
	{
	  JLiterObjectPtr r = JLiterObject::instance();	r->value="+";	r->properties[tag::FLAG_OPERATOR] = nullObject;
	  map["+"] = r;
	  r = JLiterObject::instance();	r->value="-";	r->properties[tag::FLAG_OPERATOR] = nullObject;
	  map["-"] = r;
	  r = JLiterObject::instance();	r->value="*";	r->properties[tag::FLAG_OPERATOR] = nullObject;
	  map["*"] = r;
	  r = JLiterObject::instance();	r->value="/";	r->properties[tag::FLAG_OPERATOR] = nullObject;
	  map["/"] = r;
	  r = JLiterObject::instance();	r->value="start";	r->properties[tag::FLAG_OPERATOR] = nullObject;
	  map["start"] = r;
	}	
      };
      static operatorsInit oInitializer(operators);
    }
  }
}

static bool reverse_polish(std::vector<JLiterObjectPtr>& symbols, std::vector<JLiterObjectPtr>& tempOps, const std::vector< jrun::vmachine::JRunContextPtr >& cxts, const data::dOpValue& e)
{
  //an functor to deal with operator stacks
  auto functor = [&](){
    JLiterObjectPtr op = calculator::operators[e.op];
    int prior = calculator::priorities[e.op];
    JLiterObjectPtr ti;
    while(!tempOps.empty())
    {
      ti = tempOps.back();
      int p = calculator::priorities[ti->value];
      if(prior > p) {      
      break;
      }
      symbols.push_back(ti);
      tempOps.pop_back();
    }
    tempOps.push_back(op);
  };
  //Here, we don't push e.first, en... very bad, to fit with recurse calling
  /*
  JObjectPtr v1 = boost::apply_visitor(expr_command_visitor(cxts), e.first);
  JLiterObjectPtr first = std::dynamic_pointer_cast<JLiterObject>(v1);
  if(!first.get()) goto error;
  symbols.push_back(first);
  */
  functor();	//deal with op
  if(e.grouped)
  {    
    goto work;
  }
  if( e.second.type() == typeid(data::dOpValue) )
  {
    data::dOpValue e2 = boost::get<data::dOpValue>(e.second);
    //Here, wu push e.first before calling reverse_polish
    JObjectPtr v1 = boost::apply_visitor(expr_command_visitor(cxts), e2.first);
    JLiterObjectPtr first = std::dynamic_pointer_cast<JLiterObject>(v1);
    if(!first.get()) goto error;
    symbols.push_back(first);    
    bool result = reverse_polish(symbols, tempOps, cxts, e2);
    if(!result) goto error;
    else goto out;
  }
work:
  {
    
  JObjectPtr v2 = boost::apply_visitor(right_command_visitor(cxts), e.second);
  JLiterObjectPtr second = std::dynamic_pointer_cast<JLiterObject>(v2);
  if(!second.get()) goto error;
  symbols.push_back(second);  
  //clear the rest in tempOps, because when we goes to here, meas we goes to the end
  while(tempOps.size() > 1)
  {
    JLiterObjectPtr ti = tempOps.back();
    symbols.push_back(ti);
    tempOps.pop_back();
  }
  
  }
out:
  return true;
error:
  return false;
}

static JLiterObjectPtr compute(const std::vector< jrun::vmachine::JRunContextPtr >& cxts, const std::vector<JLiterObjectPtr>& symbols){
  std::stack<JLiterObjectPtr> result;
  int t = symbols.size();
  for(int i = 0; i < t; i++)
  {
    JLiterObjectPtr o = symbols.at(i);
    if( o->properties.count(tag::FLAG_OPERATOR) > 0 )
    {
      JLiterObjectPtr r2 = result.top();
      result.pop();
      JLiterObjectPtr r1 = result.top();
      result.pop();
      JLiterObjectPtr r = calculator::functors[o->value](r1, r2);
      result.push(r);
    }
    else
    {
      result.push(o);
    }
  }
  return result.top();
}

JObjectPtr jrun::vmachine::calculate(const std::vector< jrun::vmachine::JRunContextPtr >& cxts, const data::dOpValue& e)
{  
  assert(!e.op.empty());
  std::vector<JLiterObjectPtr> symbols;	std::vector<JLiterObjectPtr> tempOps;
  tempOps.push_back(calculator::operators["start"]);
  JObjectPtr v1 = boost::apply_visitor(expr_command_visitor(cxts), e.first);
  JLiterObjectPtr first = std::dynamic_pointer_cast<JLiterObject>(v1);
  if(!first.get()) throw CalculatorException();
  symbols.push_back(first);
  bool result = reverse_polish(symbols, tempOps, cxts, e);
  {
#ifdef DEBUG  
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("dumping reverse_polish result: "));
  int st = symbols.size();
  for(int i = 0; i < st; ++i)
  {
    JLiterObjectPtr t = symbols.at(i);
    jrun::log::Logger::log(jrun::log::level::INFO, t->value);
  }
#endif
  }
  if(!result) throw CalculatorException();
  JLiterObjectPtr r = compute(cxts, symbols);
  return std::static_pointer_cast<JObject>(r);
}