#include <fstream>
#include "wrapper/wrapper.hpp"
#include "parser/interface.hpp"
#include "vmachine/vmachine.hpp"
#include "vmachine/JRunContext.hpp"

using namespace jrun::wrapper;

bool jrun::wrapper::execute(const std::string& filename, callbacks& cb)
{
  std::shared_ptr<data::AST> d(new data::AST());
  
  std::string str;  
  std::ifstream in(filename.c_str(), std::ios_base::in);
  if (!in)
  {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return false;
  }
  in.unsetf(std::ios::skipws); // No white space skipping!
  std::copy(
    std::istream_iterator<char>(in),
    std::istream_iterator<char>(),
    std::back_inserter(str));
  if(cb.beforeParse)
    if( !cb.beforeParse(str, d) ) return false;
  
  char const* begin = str.c_str();
  char const* end = &begin[str.size()];
  bool r  = jrun::parser::parse(begin, end, d);
  if(cb.afterParse)
    if( !cb.afterParse(r, d) ) return false;
  
  std::vector<jrun::vmachine::JRunContextPtr> cxts;
  cxts.push_back(jrun::vmachine::JRunContext::instance());
  if(cb.beforeRun)
    if( !cb.beforeRun(cxts, d) ) return false;
  jrun::vmachine::JObjectPtr re = jrun::vmachine::VM::execute(cxts, d);
  
  if(cb.afterRun)
    if( !cb.afterRun(re) ) return false;
  
  return true;
}

bool jrun::wrapper::registerFun(std::vector<jrun::vmachine::JRunContextPtr>& cxts, const jrun::vmachine::JFunctor & f, std::string& name, std::size_t args, jrun::vmachine::JObjectPtr obj)
{
  jrun::vmachine::JFunObjectPtr fun = jrun::vmachine::JFunObject::instance();
  fun->args = args;
  fun->operations = f;
  fun->contexts = cxts;
  obj->properties[name] = std::static_pointer_cast<jrun::vmachine::JObject>(fun); //在 定义环境 中添加函数对象，便于递归
  return true;
}


bool jrun::wrapper::registerFun(std::vector<jrun::vmachine::JRunContextPtr>& cxts, const jrun::vmachine::JFunctor & f, std::string& name, std::size_t args)
{
  return jrun::wrapper::registerFun(cxts, f, name, args, cxts.back());
}