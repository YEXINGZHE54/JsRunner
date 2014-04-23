#ifndef JRUN_VMACHINE_TAG_HPP
#define JRUN_VMACHINE_TAG_HPP

#include <string>

namespace jrun{
  namespace vmachine{
    namespace tag{
      const std::string REF_CLOSURE 	=	"__flag_ref_closure__";
      const std::string FLAG_RET	=	"__flag_return__";
      const std::string FLAG_OPERATOR	=	"__flag_operator__";
    }
  }
}

#endif