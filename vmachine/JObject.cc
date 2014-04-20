#include "vmachine/JObject.hpp"
#include "vmachine/exception.hpp"
#include <iostream>
#include <memory>

using namespace jrun::vmachine;

namespace jrun{
  namespace vmachine{
    JObjectPtr nullObject = JObject::instance();
    std::size_t counter = 0;
  }
}

JObject::JObject()
{
#ifdef DEBUG
  counter++;
  std::cerr << "now constructed object is " << counter << std::endl;
#endif
}

JObject::~JObject()
{
#ifdef DEBUG
  counter--;
  std::cerr << "now constructed object is " << counter << std::endl;
#endif
}

JObjectPtr JObject::instance()
{
  return std::shared_ptr<JObject>(new JObject());
}

JObjectPtr JObject::share()
{
  return shared_from_this();
}

void JObject::dummy()
{

}

JFunObjectPtr JFunObject::instance()
{
  return std::shared_ptr<JFunObject>(new JFunObject());
}

JLiterObjectPtr JLiterObject::instance()
{
  return std::shared_ptr<JLiterObject>(new JLiterObject());
}

JObjectPtr JObject::operator*(JObjectPtr o1)
{
  JLiterObjectPtr l1 = std::dynamic_pointer_cast<JLiterObject>(o1);
  JObjectPtr self = this->share();
  JLiterObjectPtr l = std::dynamic_pointer_cast<JLiterObject>(self);
  if(! ( l1.get() && l.get() ) )
  {
    throw CalculatorException();
  }
  std::string v = l->value;	std::string v1 = l1->value;
  JLiterObjectPtr o = JLiterObject::instance();
  if( (v.at(0) != '"') && (v1.at(0) != '"') )
  {
    long double n1 = std::stold(v);	long double n2 = std::stold(v1);
    if(n2 == 0) throw CalculatorException();	//除0错误
    std::string re = std::to_string( n1 * n2 );
    o->value = re;
  } else {
    throw CalculatorException();
  }
  return std::static_pointer_cast<JObject>(o);
}

JObjectPtr JObject::operator+(JObjectPtr o1)
{
  JLiterObjectPtr l1 = std::dynamic_pointer_cast<JLiterObject>(o1);
  JObjectPtr self = this->share();
  JLiterObjectPtr l = std::dynamic_pointer_cast<JLiterObject>(self);
  if(! ( l1.get() && l.get() ) )
  {
    throw CalculatorException();
  }
  std::string v = l->value;	std::string v1 = l1->value;
  JLiterObjectPtr o = JLiterObject::instance();
  if( (v.at(0) != '"') && (v1.at(0) != '"') )
  {
    // calculate as numbers
    long double n1 = std::stold(v);	long double n2 = std::stold(v1);
    std::string re = std::to_string( n1 + n2 );
    o->value = re;
  } else {
    std::string re = v;
    re.insert(re.end(), v1.begin() + 1, v1.end());
    o->value = re;
  };
  return std::static_pointer_cast<JObject>(o);
}
JObjectPtr JObject::operator-(JObjectPtr o1)
{
  JLiterObjectPtr l1 = std::dynamic_pointer_cast<JLiterObject>(o1);
  JObjectPtr self = this->share();
  JLiterObjectPtr l = std::dynamic_pointer_cast<JLiterObject>(self);
  if(! ( l1.get() && l.get() ) )
  {
    throw CalculatorException();
  }
  std::string v = l->value;	std::string v1 = l1->value;
  JLiterObjectPtr o = JLiterObject::instance();
  if( (v.at(0) != '"') && (v1.at(0) != '"') )
  {
    long double n1 = std::stold(v);	long double n2 = std::stold(v1);
    std::string re = std::to_string( n1 - n2 );
    o->value = re;
  } else {
    throw CalculatorException();
  }
  return std::static_pointer_cast<JObject>(o);
}

JObjectPtr JObject::operator/(JObjectPtr o1)
{
  JLiterObjectPtr l1 = std::dynamic_pointer_cast<JLiterObject>(o1);
  JObjectPtr self = this->share();
  JLiterObjectPtr l = std::dynamic_pointer_cast<JLiterObject>(self);
  if(! ( l1.get() && l.get() ) )
  {
    throw CalculatorException();
  }
  std::string v = l->value;	std::string v1 = l1->value;
  JLiterObjectPtr o = JLiterObject::instance();
  if( (v.at(0) != '"') && (v1.at(0) != '"') )
  {
    long double n1 = std::stold(v);	long double n2 = std::stold(v1);
    if(n2 == 0) throw CalculatorException();	//除0错误
    std::string re = std::to_string( n1 / n2 );
    o->value = re;
  } else {
    throw CalculatorException();
  }
  return std::static_pointer_cast<JObject>(o);
}

