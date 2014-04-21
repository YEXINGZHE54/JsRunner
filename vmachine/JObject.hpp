#ifndef JRUN_VMACHINE_JOBJECT_HPP
#define JRUN_VMACHINE_JOBJECT_HPP

#include <memory>
#include <map>
#include "parser/generation.hpp"
#include "vmachine/JRunContext.hpp"

namespace jrun{
  namespace vmachine{
    class JObject;
    typedef std::shared_ptr<JObject> JObjectPtr;    
    typedef std::string propertyName;
    typedef JObjectPtr propertyValue;
    
    class JObject : public std::enable_shared_from_this<JObject>{
    public:
      JObject();
      ~JObject();
      static JObjectPtr instance();
      JObjectPtr share();
      std::map<propertyName, propertyValue> properties;
      JObjectPtr operator+ (JObjectPtr o1);
      JObjectPtr operator- (JObjectPtr o1);
      JObjectPtr operator* (JObjectPtr o1);
      JObjectPtr operator/ (JObjectPtr o1);
      bool isTrue();
      virtual void dummy();
    };
    
    class Operation;	class JFunObject;
    typedef std::shared_ptr<JFunObject> JFunObjectPtr;
    class JFunObject : public JObject{
    public:
      static JFunObjectPtr instance();   
      std::vector<JRunContextPtr> contexts;	//继承自 函数生成时 的执行环境链条， 执行时需要再加入一个context
      std::vector<std::string> args;
      std::vector<jrun::generation::mCommand> operations;
    };
    
    class JLiterObject; typedef std::shared_ptr<JLiterObject> JLiterObjectPtr;
    class JLiterObject : public JObject{
    public:
      static JLiterObjectPtr instance();  
      std::string value;
    };
    extern JObjectPtr nullObject;
  }
}

#endif