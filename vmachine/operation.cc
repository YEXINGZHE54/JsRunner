#include "vmachine/operation.hpp"
#include "assert.h"
#include "evaluate/visitor.hpp"
#include <boost/variant/apply_visitor.hpp>
#include "vmachine/exception.hpp"
#include "vmachine/JRunContext.hpp"
#include "vmachine/vmachine.hpp"
#include <iostream>
#include <memory>
#include "log/logger.hpp"
#include "vmachine/tag.hpp"
#include "vmachine/closure.hpp"
#include "evaluate/calculator.hpp"

using namespace jrun::vmachine;

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::NamedFunc& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("defining function in operation  ") + f.name);
#endif
  assert(!cxts.empty());
  JRunContextPtr cur = cxts.back();
  JFunObjectPtr fun = JFunObject::instance();
  cur->properties[f.name] = std::static_pointer_cast<JObject>(fun); //在 定义环境 中添加函数对象，便于递归
  
  fun->args = f.argsv.size();  
  fun->contexts = cxts;
  
  auto func = [f](const std::vector<JRunContextPtr> &scopeChain, const std::vector<JObjectPtr> &args)->JObjectPtr
  {        
    //now prepare for function to run
    std::size_t n = args.size();
    JRunContextPtr c = JRunContext::instance();
    for(std::size_t i = 0; i < n; ++i)
    {
      JObjectPtr v = args.at(i);
      std::string k = f.argsv.at(i);
      c->properties[k] = v;
    };
  
    //link cxts
    std::vector<JRunContextPtr> newScopeChain = scopeChain;  
    newScopeChain.push_back(c);
    
    JObjectPtr re;
    try {    
      re = VM::runCommands(newScopeChain, f.commands);
    } catch (ReturnKit r) {
      re = r.result;
    }
    
    //closure deal
    for(jrun::vmachine::JRunContext::mapIterator it = c->properties.begin(); it != c->properties.end(); ++it)
    {
      if( !( it->second->properties.count(jrun::vmachine::tag::REF_CLOSURE) > 0 ) )	//没有被闭包所引用
	c->properties.erase(it);	//清除所有当前变量
    }    
    return re;
  };
  fun->operations = func;
  
  // check for posssible closure references
  if(cxts.size() == 1) goto out;		//if in global envrionment, no check
  {
  std::vector<JRunContextPtr> contexts;	//temeroray contexts
  contexts.push_back(cxts.back());
  JRunContextPtr c = JRunContext::instance();
  int t = f.argsv.size();
  for(int i = 0; i < t; ++i)
  {
    c->properties[f.argsv.at(i)] = nullObject;
  }
  contexts.push_back(c);			//Func has args, so need to init current environments
  jrun::vmachine::closure_ref_test(f.commands, contexts);
  }
out:  
  return nullObject;
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::retCommand& ret)
{
  JObjectPtr ptr = boost::apply_visitor(right_command_visitor(cxts), ret.exr);
  throw ReturnKit(ptr);		//we use throw try-catch to jump out of deep stackFrames, maybe it is the only way to do it
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::ifCommand& r)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("if command blocks in operation  ") );
#endif
  JObjectPtr condition = boost::apply_visitor(expr_command_visitor(cxts) , r.e);
  JObjectPtr result = nullObject;
  if(condition->isTrue()) {
    result = VM::runCommands(cxts, r.commands);
  } else {
    result = VM::runCommands(cxts, r.elsecoms);
  }
  return result;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::forCommand& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("for command blocks in operation  ") );
#endif
  for(std::vector<jrun::generation::Expr>::const_iterator it = f.start.begin(); it != f.start.end(); ++it)
  {
    boost::apply_visitor(expr_command_visitor(cxts), *it);
  }
  JObjectPtr result = nullObject;
  JObjectPtr condition = boost::apply_visitor(expr_command_visitor(cxts), f.condition);
  while(condition->isTrue())
  {
    try{
      result = VM::runCommands(cxts, f.commands);
    } catch (BreakKit b) {
      return result;
    }
    for(std::vector<jrun::generation::Expr>::const_iterator it = f.after.begin(); it != f.after.end(); ++it)
    {
      boost::apply_visitor(expr_command_visitor(cxts), *it);
    }
    condition = boost::apply_visitor(expr_command_visitor(cxts), f.condition);
  }
  return result;
}

JObjectPtr Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::whileCommand& w)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("while command blocks in operation  ") );
#endif
  JObjectPtr result = nullObject;
  JObjectPtr condition = boost::apply_visitor(expr_command_visitor(cxts), w.condition);
  while(condition->isTrue())
  {
    try{
      result = VM::runCommands(cxts, w.commands);
    } catch (BreakKit b) {
      return result;
    }
    condition = boost::apply_visitor(expr_command_visitor(cxts), w.condition);
  }
  return result;
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::Assign& ass)
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

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::AnnoFunc& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("AnnoFunc in operation"));
#endif
  assert(!cxts.empty());
  JFunObjectPtr fun = JFunObject::instance();

  fun->args = f.argsv.size();
  fun->contexts = cxts;
  
  auto func = [f](const std::vector<JRunContextPtr> &scopeChain, const std::vector<JObjectPtr> &args)->JObjectPtr
  {        
    //now prepare for function to run
    std::size_t n = args.size();
    JRunContextPtr c = JRunContext::instance();
    for(std::size_t i = 0; i < n; ++i)
    {
      JObjectPtr v = args.at(i);
      std::string k = f.argsv.at(i);
      c->properties[k] = v;
    };
  
    //link cxts
    std::vector<JRunContextPtr> newScopeChain = scopeChain;  
    newScopeChain.push_back(c);
    
    JObjectPtr re;
    try {    
      re = VM::runCommands(newScopeChain, f.commands);
    } catch (ReturnKit r) {
      re = r.result;
    }
    
    //closure deal
    for(jrun::vmachine::JRunContext::mapIterator it = c->properties.begin(); it != c->properties.end(); ++it)
    {
      if( !( it->second->properties.count(jrun::vmachine::tag::REF_CLOSURE) > 0 ) )	//没有被闭包所引用
	c->properties.erase(it);	//清除所有当前变量
    }    
    return re;
  };
  fun->operations = func;
  
  // check for posssible closure references
  if(cxts.size() == 1) goto out;
  {
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
  }
out:
  return std::static_pointer_cast<JObject>(fun);
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::dOpValue& dOp)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("dOpValue in operation"));
#endif
  assert(!cxts.empty());
  return calculate(cxts, dOp);
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& , const jrun::generation::sOpValue& )
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("sOp in operation"));
#endif
  //TODO
  return nullObject;
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::Objectdef& obj)
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

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::funCall& f)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("FuncCall in operation"));
#endif
  assert(!cxts.empty());
  JFunObjectPtr fun = std::dynamic_pointer_cast<JFunObject>( boost::apply_visitor(left_command_visitor(cxts), f.name) );
  if( !(fun.get()) ) throw funNotfoundException();	//cannot convirt to function
  if(fun->args != f.args.size()) throw funArgException();  
  
  //push args in vector
  std::size_t n = fun->args;
  std::vector<JObjectPtr> args;
  for(std::size_t i = 0; i < n; i++)
  {
    JObjectPtr v = boost::apply_visitor(right_command_visitor(cxts), f.args.at(i));
    args.push_back(v);
  };
  
  //run it!
  return fun->operations(fun->contexts, args);
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::literValue& v)
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("literValue in operation") + v);
#endif
  assert(!cxts.empty());
  JLiterObjectPtr o = JLiterObject::instance();
  o->value = v;
  return std::static_pointer_cast<JObject>(o);
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::names& names)
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

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< JRunContextPtr >& cxts, const jrun::generation::mapKey& map)
{
  jrun::generation::leftValue lvalue(map);
  return exec(cxts, boost::apply_visitor(leftname_visitor(), lvalue));
  //return nullObject;
}

JObjectPtr jrun::vmachine::Operation::exec(const std::vector< jrun::vmachine::JRunContextPtr>& cxts, const jrun::generation::mapConst& map)
{
  jrun::generation::leftValue lvalue(map);
  return exec(cxts, boost::apply_visitor(leftname_visitor(), lvalue));
}
