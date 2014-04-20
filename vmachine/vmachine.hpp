#ifndef JRUN_VMACHINE_HPP
#define JRUN_VMACHINE_HPP

#include "parser/generation.hpp"
#include <memory>
#include "vmachine/JObject.hpp"

namespace jrun{
  namespace vmachine {    
    namespace gen = jrun::generation;  
    
    class VM{
    public:
      VM();
      static void execute(std::shared_ptr<gen::AST> commands);
      static JObjectPtr runCommands(const std::vector<JRunContextPtr>& scopeChain, const std::vector<gen::mCommand>& commands);
    };
    
  }
}

#endif