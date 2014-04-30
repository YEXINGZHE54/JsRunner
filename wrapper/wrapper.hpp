#ifndef JRUN_WRAPPER_HPP
#define JRUN_WRAPPER_HPP
#include <functional>
#include <string>
#include <memory>
#include "parser/generation.hpp"
#include "vmachine/JObject.hpp"

namespace jrun{
  namespace wrapper{
    namespace data = jrun::generation;
    struct callbacks
    {
      std::function<bool(std::string&, std::shared_ptr<data::AST>)> beforeParse;
      std::function<bool(bool, std::shared_ptr<data::AST>)> afterParse;
      std::function<bool(std::vector<jrun::vmachine::JRunContextPtr>&, std::shared_ptr<data::AST>)> beforeRun;
      std::function<bool(jrun::vmachine::JObjectPtr)> afterRun;
    };
    bool execute(const std::string& filename, callbacks& cb);
    bool registerFun(std::vector<jrun::vmachine::JRunContextPtr>& cxts, const jrun::vmachine::JFunctor & f, std::string& name, std::size_t args);
    bool registerFun(std::vector<jrun::vmachine::JRunContextPtr>& cxts, const jrun::vmachine::JFunctor & f, std::string& name, std::size_t args, jrun::vmachine::JObjectPtr obj);
  }
}

#endif