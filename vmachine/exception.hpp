#ifndef JRUN_VMACHINE_EXCEPTION_HPP
#define JRUN_VMACHINE_EXCEPTION_HPP

#include <exception>
#include "vmachine/JObject.hpp"

namespace jrun{
  namespace vmachine{
    class mException : public std::exception{
    public:
      virtual const char* what() const throw()
      {
	return "vMachine runtime exception";
      }
    };
    
    class funNotfoundException : public std::exception {
    public:
      virtual const char* what() const throw()
      {
	return "function Not Found!";
      }
    };
    
    class funArgException : public std::exception {
    public:
      virtual const char* what() const throw()
      {
	return "function args not compatitable";
      }
    };
    
    class UndefinedException : public std::exception {
    public:
      virtual const char* what() const throw()
      {
	return "undefined variable!";
      }
    };
    
    class CalculatorException : public std::exception {
    public:
      virtual const char* what() const throw()
      {
	return "variable connot be calculated!";
      }
    };
    
    class ReturnKit{
    public:
    ReturnKit(JObjectPtr ptr) : result(ptr){};
    JObjectPtr result;
    };
    
    class BreakKit{
      
    };
  }
}

#endif