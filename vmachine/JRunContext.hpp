#ifndef JRUN_VMACHINE_JCONTEXT_HPP
#define JRUN_VMACHINE_JCONTEXT_HPP

#include <vector>
#include <memory>
#include <map>

namespace jrun{
  namespace vmachine{
    class JObject;
    typedef std::shared_ptr<JObject> JRunContextPtr;    
    
    class JRunContext {
    public:
      typedef std::map<std::string, std::shared_ptr<JObject> >::iterator mapIterator;
      static const JRunContextPtr instance();
    };
  }
}

#endif