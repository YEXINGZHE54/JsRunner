#include "vmachine/closure.hpp"
#include "vmachine/tag.hpp"
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include "parser/generation.hpp"
#include "vmachine/JObject.hpp"
#include "assert.h"

namespace jrun{
  namespace vmachine {
    namespace gen = jrun::generation;
    class closure_visitor : public boost::static_visitor<>{
      public:
	closure_visitor(const std::vector<JRunContextPtr>& cxts) : contexts(cxts) {};
	void operator()(const gen::Expr&) const;
	void operator()(const gen::NamedFunc&) const;
	void operator()(const gen::retCommand&) const;	
	void operator()(const gen::Assign&) const;
	void operator()(const gen::rightValue&) const;
	void operator()(const gen::tValue&) const;
	void operator()(const gen::dOpValue&) const;
	void operator()(const gen::sOpValue&) const;
	void operator()(const gen::funCall&) const;
      	void operator()(const gen::Objectdef&) const;
      	void operator()(const gen::AnnoFunc&) const;      
      	void operator()(const gen::leftValue&) const;
      	void operator()(const gen::ifCommand&) const;
      	void operator()(const gen::names&) const;
      	void operator()(const gen::mapKey&) const;
      	void operator()(const gen::mapConst&) const;
	void operator()(const std::string&) const;
    private:
	const std::vector<JRunContextPtr>& contexts;
    };       
  }
}

using namespace jrun::vmachine;

void closure_visitor::operator() ( const jrun::generation::Expr& e ) const
{
  boost::apply_visitor(closure_visitor(contexts), e);
}

void closure_visitor::operator() ( const jrun::generation::NamedFunc& f ) const
{
  std::vector<JRunContextPtr> newCxts = contexts;
  JRunContextPtr cur = JRunContext::instance();
  int t = f.argsv.size();
  for(int i = 0; i < t; ++i)
  {
    cur->properties[f.argsv.at(i)] = nullObject;
  }
  newCxts.push_back(cur);
  closure_ref_test(f.commands, newCxts);  
}

void closure_visitor::operator() ( const jrun::generation::retCommand& e) const
{
  boost::apply_visitor(closure_visitor(contexts), e.exr);
}

void closure_visitor::operator()(const jrun::generation::ifCommand& r) const
{
  boost::apply_visitor(closure_visitor(contexts), r.e);
  jrun::vmachine::closure_ref_test(r.commands, contexts);
  jrun::vmachine::closure_ref_test(r.elsecoms, contexts);
}

void closure_visitor::operator() ( const jrun::generation::Assign& e) const
{
  boost::apply_visitor(closure_visitor(contexts), e.key);
  boost::apply_visitor(closure_visitor(contexts), e.value);
}

void closure_visitor::operator() ( const jrun::generation::rightValue& e) const
{
  boost::apply_visitor(closure_visitor(contexts), e);
}

void closure_visitor::operator()(const jrun::generation::tValue& e) const
{
  boost::apply_visitor(closure_visitor(contexts), e);
}

void closure_visitor::operator() ( const jrun::generation::dOpValue& e) const
{
  boost::apply_visitor(closure_visitor(contexts), e.first);
  boost::apply_visitor(closure_visitor(contexts), e.second);
}

void closure_visitor::operator() ( const jrun::generation::sOpValue& e ) const
{
  boost::apply_visitor(closure_visitor(contexts), e.first);
}

void closure_visitor::operator() ( const jrun::generation::funCall& f ) const
{
  int sizet = f.args.size();
  for(int i  = 0; i < sizet; ++i)
  {
    boost::apply_visitor(closure_visitor(contexts), f.args.at(i));
  }
  boost::apply_visitor(closure_visitor(contexts), f.name);
}

void closure_visitor::operator() ( const jrun::generation::Objectdef& e ) const
{
  int sizet = e.properties.size();
  for(int i = 0; i < sizet; ++i)
  {
    jrun::generation::propertyAssign as = e.properties.at(i);
    boost::apply_visitor(closure_visitor(contexts), as.value);
  }
}

void closure_visitor::operator() ( const jrun::generation::AnnoFunc&  f) const
{
  std::vector<JRunContextPtr> newCxts = contexts;
  JRunContextPtr cur = JRunContext::instance();
  int t = f.argsv.size();
  for(int i = 0; i < t; ++i)
  {
    cur->properties[f.argsv.at(i)] = nullObject;
  }
  newCxts.push_back(cur);
  closure_ref_test(f.commands, newCxts);  
}

void closure_visitor::operator() ( const jrun::generation::leftValue& e ) const
{
  boost::apply_visitor(closure_visitor(contexts), e );
}

void closure_visitor::operator() ( const jrun::generation::names& e ) const
{
  this->operator()(e.front());
}

void closure_visitor::operator() ( const jrun::generation::mapKey& e ) const
{
  this->operator()(e.map.front());
  this->operator()(e.key.front());
}

void closure_visitor::operator() ( const jrun::generation::mapConst& e ) const
{
  this->operator()(e.map.front());
}

void closure_visitor::operator() ( const std::string& name ) const
{
  if(name.empty() || name.at(0) == '"') return;
  //first search in current, if it has appeared before, then it must have been placed in current environment
  JRunContextPtr j = contexts.back();
  if( j->properties.count(name) > 0) return;
  j->properties[name] = nullObject;	//it appeared first time, put in in current, wherever should it be
  
  int sizet = contexts.size();
  for(int i = sizet - 2; i >= 1; --i) //first one is parent, skip i = 0 , i = sizet-1
  {
    j = contexts.at(i);
    if(j->properties.count(name) > 0) return;
  }
  
  j = contexts.front();			//now, deal with parent referred var
  if(j->properties.count(name) > 0)
  {
    JObjectPtr o = j->properties[name];
    o->properties[tag::REF_CLOSURE] = nullObject;	//mark it in parent environment as refered by closure
    return;
  }
}

/*
 * A Helper Function, maybe not effective
 * To test whether variables in parent environment are referred by commands in Annonymous Function
 * so, the front of vector is the parent, regardless of rest elements
 * Note : we need to prepare arguments in current envrionment ourselves, not in helper function
 */
void jrun::vmachine::closure_ref_test (const std::vector< jrun::generation::mCommand >& commands,const std::vector<JRunContextPtr>& cxts )
{  
  assert( cxts.size() > 1 );
  for(std::vector<jrun::generation::mCommand>::const_iterator it = commands.begin(); it != commands.end(); ++it){
    boost::apply_visitor(closure_visitor(cxts), (*it));
  }
}