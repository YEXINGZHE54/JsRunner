#include <iostream>
#include <string>
#include "vmachine.hpp"
#include <boost/variant/apply_visitor.hpp>
#include "vmachine/operation.hpp"
#include "evaluate/visitor.hpp"
#include "vmachine/exception.hpp"
#include "vmachine/JObject.hpp"
#include "log/logger.hpp"

using namespace jrun::vmachine;

void VM::execute(std::shared_ptr< jrun::generation::AST > commands)
{
  std::vector<JRunContextPtr> cxts;
  cxts.push_back( JObject::instance() );
  JObjectPtr r = nullObject;
  try{
    r = runCommands(cxts, commands->commands);
  } catch (std::exception& e) {
      std::cerr << e.what() <<std::endl;
  } catch(...) {
      std::cerr << "Unsupported operation!" <<std::endl;
  }
  
  JRunContextPtr cur = cxts.front();
  for(jrun::vmachine::JRunContext::mapIterator it = cur->properties.begin(); it != cur->properties.end(); ++it)
  {
    cur->properties.erase(it);	//清除所有当前变量
  }
  
  if(r == nullObject ) return;
  try{
    JLiterObjectPtr re = std::dynamic_pointer_cast<JLiterObject>(r);
    if(re.get())
      std::cout << "final value is : " << re->value << std::endl;
    else
      std::cerr << "not an Literal value!" << std::endl;
    return;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  };
};

JObjectPtr VM::runCommands(const std::vector< JRunContextPtr >& scopeChain, const std::vector< jrun::generation::mCommand >& commands)
{
#ifdef DEBUG
    jrun::log::Logger::log(jrun::log::level::INFO, std::string("commands total count in vmachine : ") + std::to_string(commands.size() ) );
#endif
  JObjectPtr result = nullObject;
  for(std::vector< jrun::generation::mCommand >::const_iterator it = commands.begin(); it != commands.end(); ++it)
  {
#ifdef DEBUG
    jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a command in vmachine!") );
#endif
    result = boost::apply_visitor(command_visitor(scopeChain), (*it));
  };
  return result;
}
