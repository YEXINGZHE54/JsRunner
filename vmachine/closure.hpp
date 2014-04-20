#ifndef JRUN_VMACHINE_CLOSURE_HPP
#define JRUN_VMACHINE_CLOSURE_HPP

#include <vector>
#include "parser/generation.hpp"
#include "vmachine/JRunContext.hpp"

namespace jrun{
  namespace vmachine{
    void closure_ref_test(const std::vector<generation::mCommand>& commands,const std::vector<JRunContextPtr>& cxts);
  }
}

#endif