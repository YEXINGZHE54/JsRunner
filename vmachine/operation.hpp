#ifndef JRUN_VMACHINE_OPERATION_HPP
#define JRUN_VMACHINE_OPERATION_HPP

#include "parser/generation.hpp"
#include "vmachine/JObject.hpp"
#include "vmachine/JRunContext.hpp"

namespace jrun{
  namespace vmachine{
    namespace gen = jrun::generation;
    
    class Operation {
    public:
      Operation();  
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::NamedFunc &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::retCommand &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::Assign &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::AnnoFunc &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::dOpValue &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::sOpValue &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::Objectdef &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::funCall &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::literValue &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::names &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::mapKey &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::mapConst &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::ifCommand &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::forCommand &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::whileCommand &);
      static JObjectPtr exec(std::vector<JRunContextPtr> const&, const gen::breakCommand &);
    };
  }
}
    
#endif