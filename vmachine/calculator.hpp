#ifndef JRUN_VMACHINE_CALCULATOR_HPP
#define JRUN_VMACHINE_CALCULATOR_HPP

#include <vector>
#include "vmachine/JRunContext.hpp"
#include "parser/generation.hpp"
#include "vmachine/JObject.hpp"

namespace jrun{
  namespace vmachine{
    JObjectPtr calculate( const std::vector<JRunContextPtr>& cxts, const jrun::generation::dOpValue& e );
  }
}

#endif