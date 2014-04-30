#ifndef JRUN_VMACHINE_VISITOR_HPP
#define JRUN_VMACHINE_VISITOR_HPP

#include <boost/variant/static_visitor.hpp>
#include "parser/generation.hpp"
#include "vmachine/JObject.hpp"
#include "vmachine/JRunContext.hpp"

namespace jrun {
  namespace vmachine {    
    namespace gen = jrun::generation;
    class command_visitor : public boost::static_visitor< JObjectPtr >{
      public:
	command_visitor(const std::vector<JRunContextPtr>&);
	JObjectPtr operator()(const gen::Expr&) const;
	JObjectPtr operator()(const gen::NamedFunc&) const;
	JObjectPtr operator()(const gen::retCommand&) const;	
	JObjectPtr operator()(const gen::ifCommand& ) const;
	JObjectPtr operator()(const gen::forCommand& ) const;
	JObjectPtr operator()(const gen::whileCommand& ) const;
	JObjectPtr operator()(const gen::breakCommand& ) const;
    private:
	const std::vector<JRunContextPtr>& contexts;
    };
    
    class leftname_visitor : public boost::static_visitor< gen::names > {
    public:
      gen::names operator() (const gen::names& name) const;
      gen::names operator() (const gen::mapKey& name) const;
      gen::names operator() (const gen::mapConst& name) const;
    };
    
    class expr_command_visitor : public boost::static_visitor< JObjectPtr >{
    public:
      expr_command_visitor(const std::vector<JRunContextPtr>&);
      JObjectPtr operator()(const gen::Assign&) const;
      JObjectPtr operator()(const gen::rightValue&) const;
      JObjectPtr operator()(const gen::tValue&) const;
    private:
	const std::vector<JRunContextPtr>& contexts;
    };
    
    class right_command_visitor : public boost::static_visitor< JObjectPtr >{
    public:
      right_command_visitor(const std::vector<JRunContextPtr>&);
      JObjectPtr operator()(const gen::dOpValue&) const;
      JObjectPtr operator()(const gen::sOpValue&) const;
      JObjectPtr operator()(const gen::funCall&) const;
      JObjectPtr operator()(const gen::Objectdef&) const;
      JObjectPtr operator()(const gen::AnnoFunc&) const;      
      JObjectPtr operator()(const gen::leftValue&) const;
      JObjectPtr operator()(const gen::literValue&) const;      
    private:
	const std::vector<JRunContextPtr>& contexts;
    };
    
    class left_command_visitor : public boost::static_visitor< JObjectPtr >{
    public:
      left_command_visitor(const std::vector<JRunContextPtr>&);
      JObjectPtr operator()(const gen::names&) const;
      JObjectPtr operator()(const gen::mapKey&) const;
      JObjectPtr operator()(const gen::mapConst&) const;
    private:
      const std::vector<JRunContextPtr>& contexts;
    };
  }
}

#endif