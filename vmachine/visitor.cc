#include "visitor.hpp"
#include <boost/variant/apply_visitor.hpp>
#include "vmachine/operation.hpp"
#include "log/logger.hpp"

using namespace jrun::vmachine;

command_visitor::command_visitor(const std::vector< JRunContextPtr >& cxts) : contexts(cxts)
{

}

expr_command_visitor::expr_command_visitor(const std::vector< JRunContextPtr >& cxts) : contexts(cxts)
{

}

right_command_visitor::right_command_visitor(const std::vector< JRunContextPtr >& cxts) : contexts(cxts)
{

}

left_command_visitor::left_command_visitor(const std::vector< JRunContextPtr >& cxts) : contexts(cxts)
{

}

JObjectPtr command_visitor::operator()(const jrun::generation::Expr& expr) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a expr command in command visitor") );
#endif
  return boost::apply_visitor(expr_command_visitor(contexts) , expr);
}

JObjectPtr command_visitor::operator()(const jrun::generation::NamedFunc& f) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a DEFFun command in command visitor") );
#endif
  return Operation::exec(contexts, f);
}

JObjectPtr command_visitor::operator()(const jrun::generation::retCommand& com) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a RET command in command visitor") );
#endif
  return Operation::exec(contexts, com);
}

JObjectPtr expr_command_visitor::operator()(const jrun::generation::Assign& ass) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a Assign command in expr visitor") );
#endif
  return Operation::exec(contexts, ass);
}

JObjectPtr expr_command_visitor::operator()(const jrun::generation::rightValue& expr) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a rightvalue command in expr visitor") );
#endif
  return boost::apply_visitor(right_command_visitor(contexts) , expr);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::AnnoFunc& anF) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a AnnomousDefFun command in right visitor") );
#endif
  return Operation::exec(contexts, anF);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::dOpValue& dOp) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a dOp command in right visitor") );
#endif
  return Operation::exec(contexts, dOp);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::sOpValue& sOp) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a sOp command in right visitor") );
#endif
  return Operation::exec(contexts, sOp);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::Objectdef& obj) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a ObjectDef command in right visitor") );
#endif
  return Operation::exec(contexts, obj);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::leftValue& expr) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a leftValue command in right visitor") );
#endif
  return boost::apply_visitor(left_command_visitor(contexts) , expr);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::funCall& call) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a FuncCall command in right visitor") );
#endif
  return Operation::exec(contexts, call);
}

JObjectPtr right_command_visitor::operator()(const jrun::generation::literValue& liter) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a litter command in right visitor") );
#endif
  return Operation::exec(contexts, liter);
}

JObjectPtr left_command_visitor::operator()(const jrun::generation::names& names) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a names command in left visitor") );
#endif
  return Operation::exec(contexts, names);
}

JObjectPtr left_command_visitor::operator()(const jrun::generation::mapKey& mKey) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a mapkey command in left visitor") );
#endif
  return Operation::exec(contexts, mKey);
}

JObjectPtr left_command_visitor::operator()(const jrun::generation::mapConst& mConst) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("running a mapConst command in left visitor") );
#endif
  return Operation::exec(contexts, mConst);
}

jrun::generation::names leftname_visitor::operator()(const jrun::generation::names& name) const
{
  return name;
}

jrun::generation::names leftname_visitor::operator()(const jrun::generation::mapKey& name) const
{
#ifdef DEBUG
  jrun::log::Logger::log(jrun::log::level::INFO, std::string("converting mapkey to names in leftname visitor") );
#endif
  gen::names m = name.map;
  m.reserve(m.size() + name.key.size()); //处于效率考虑， 预先分配足够空间
  m.insert(m.end(), name.key.begin(), name.key.end());
  return m;
}

jrun::generation::names leftname_visitor::operator()(const jrun::generation::mapConst& name) const
{
#ifdef DEBUG
    jrun::log::Logger::log(jrun::log::level::INFO, std::string("converting mapConst to names in leftname visitor") );
#endif
  gen::names m = name.map;
  m.push_back(name.key);
  return m;
}
