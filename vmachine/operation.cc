#include "vmachine/operation.hpp"
#include "assert.h"
#include "vmachine/visitor.hpp"
#include <boost/variant/apply_visitor.hpp>
#include "vmachine/exception.hpp"
#include "vmachine/JRunContext.hpp"
#include "vmachine/vmachine.hpp"
#include <iostream>
#include <memory>
#include "log/logger.hpp"
#include "vmachine/tag.hpp"
#include "vmachine/closure.hpp"

using namespace jrun::vmachine;

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::NamedFunc& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("defining function in operation  ") + f.name);
#endif
  assert(!cxts.empty());
  JRunContextPtr cur = cxts.back();
  JFunObjectPtr fun = JFunObject::instance();
  cur->properties[f.name] = std::static_pointer_cast<JObject>(fun); //在 定义环境 中添加函数对象，便于递归
  
  fun->args = f.argsv;
  fun->operations = f.commands;
  fun->contexts = cxts;
  //cur->properties.insert( std::pair<propertyName, propertyValue>(f.name, fun) );
  return nullObject;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::retCommand& ret)
{
  return boost::apply_visitor(right_command_visitor(cxts), ret.exr);
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::Assign& ass)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("assign key value in operation") );
#endif
  assert(!cxts.empty());  
  JObjectPtr v = boost::apply_visitor(right_command_visitor(cxts), ass.value);
  gen::names k = boost::apply_visitor(leftname_visitor(), ass.key);
  
  assert(!k.empty());
  std::string name = k.back(); k.pop_back();
  
  JObjectPtr cur;
  if(k.empty()) cur = cxts.back();
  else cur = Operation::exec(cxts, k);
  
  if(cur == nullObject) { //names refer to an undefined object
    throw UndefinedException();
    return nullObject;
  };
  
  cur->properties[name] = v;
  return v;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::AnnoFunc& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("AnnoFunc in operation"));
#endif
  assert(!cxts.empty());
  JFunObjectPtr fun = JFunObject::instance();

  fun->args = f.argsv;
  fun->operations = f.commands;
  fun->contexts = cxts;
  
  // check for posssible closure references
  std::vector<JRunContextPtr> contexts;	//temeroray contexts
  contexts.push_back(cxts.back());
  JRunContextPtr c = JRunContext::instance();
  int t = f.argsv.size();
  for(int i = 0; i < t; ++i)
  {
    c->properties[f.argsv.at(i)] = nullObject;
  }
  contexts.push_back(c);			//AnnoFunc has args, so need to init current environments
  jrun::vmachine::closure_ref_test(f.commands, contexts);

  return std::static_pointer_cast<JObject>(fun);
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::dOpValue& dOp)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("dOpValue in operation"));
#endif
  assert(!cxts.empty());

  JObjectPtr v1 = boost::apply_visitor(right_command_visitor(cxts), dOp.first);
  JObjectPtr v2 = boost::apply_visitor(right_command_visitor(cxts), dOp.second);
  assert(!dOp.op.empty());
  char c = dOp.op.at(0);
  switch(c){
    case '+':
      return v1->operator+(v2); break;
    case '-':
      return v1->operator-(v2); break;
    case '*':
      return v1->operator*(v2); break;
    case '/':
      return v1->operator/(v2); break;
    default:
      return nullObject; break;
  };
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& , const jrun::generation::sOpValue& )
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("sOp in operation"));
#endif
  //TODO
  return nullObject;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::Objectdef& obj)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("objectDef in operation"));
#endif
  assert(!cxts.empty());
  JObjectPtr o = JObject::instance();
  if(!obj.properties.empty()){    
    for(std::vector<gen::propertyAssign>::const_iterator it = obj.properties.begin(); it != obj.properties.end(); ++it)
    {
      std::string name = (*it).key;
      JObjectPtr v = boost::apply_visitor(right_command_visitor(cxts), (*it).value);
      o->properties[name] = v;
    };
  };
  return o;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::funCall& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("FuncCall in operation"));
#endif
  assert(!cxts.empty());
  JFunObjectPtr fun = std::dynamic_pointer_cast<JFunObject>( boost::apply_visitor(left_command_visitor(cxts), f.name) );
  if( !(fun.get()) ) throw funNotfoundException();	//cannot convirt to function
  if(fun->args.size() != f.args.size()) throw funArgException();
  
  //now prepare for function to run
  std::size_t n = fun->args.size();
  JRunContextPtr cur = JRunContext::instance();
  for(std::size_t i = 0; i < n; i++)
  {
    JObjectPtr v = boost::apply_visitor(right_command_visitor(cxts), f.args.at(i));
    std::string k = fun->args.at(i);
    cur->properties[k] = v;
  };
  
  //link cxts
  std::vector<JRunContextPtr> scopeChain = fun->contexts;  
  scopeChain.push_back(cur);
  
  //run it!
  JObjectPtr re = VM::runCommands(scopeChain, fun->operations);
  
  //closure deal
  for(jrun::vmachine::JRunContext::mapIterator it = cur->properties.begin(); it != cur->properties.end(); ++it)
  {
    if( !( it->second->properties.count(jrun::vmachine::tag::REF_CLOSURE) > 0 ) )	//没有被闭包所引用
      cur->properties.erase(it);	//清除所有当前变量
  }
  
  return re;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::literValue& v)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("literValue in operation") + v);
#endif
  assert(!cxts.empty());
  JLiterObjectPtr o = JLiterObject::instance();
  o->value = v;
  return std::static_pointer_cast<JObject>(o);
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::names& names)
{
  assert(!cxts.empty());
  JObjectPtr result = nullObject;
  
  if(names.empty()) return result;
  std::size_t size = names.size();
  std::string kName = names.at(0);
  
  //for(std::vector< JRunContextPtr >::reverse_iterator cxtIt = cxts.rbegin(); cxtIt != cxts.rend(); ++cxtIt)
  for(int i =cxts.size()-1; i >= 0; --i)
  {
      JRunContextPtr cxtIt = cxts.at(i);
      if( cxtIt->properties.count(kName) > 0 )
      {
	result = cxtIt->properties[kName];
	break;
      };
  };
  
  for(std::size_t i = 1; i < size; ++i)
  {
    if(result == nullObject) { //names refer to an undefined object
      throw UndefinedException();
    };
    
    std::string name = names.at(i);
    if( result->properties.count(name) > 0 )
    {
      result = result->properties[name];
    } else {
      result = nullObject;
    }
  };  
  
  return result;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::mapKey& map)
{
  jrun::generation::leftValue lvalue(map);
  return exec(cxts, boost::apply_visitor(leftname_visitor(), lvalue));
  //return nullObject;
}

JObjectPtr Operation::exec(const std::vector< jrun::vmachine::JRunContextPtr>& cxts, const jrun::generation::mapConst& map)
{
  jrun::generation::leftValue lvalue(map);
  return exec(cxts, boost::apply_visitor(leftname_visitor(), lvalue));
}
