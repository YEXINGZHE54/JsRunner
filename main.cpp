#include <iostream>
#include "wrapper/wrapper.hpp"

using namespace std;
using namespace boost;

int main(int argc, char **argv) {
    namespace data = jrun::generation;
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        filename = "data.txt";
    }

    jrun::wrapper::callbacks cb;
    cb.afterParse = [](bool r, std::shared_ptr<data::AST>)->bool
    {
      if(r) 
      {
	std::cout << "Success!" << std::endl;
	return true;
      }
      else
      {
	std::cout << "Fail!" << std::endl;
	return false;
      }
    };
    jrun::vmachine::JFunctor add = [](const std::vector<jrun::vmachine::JRunContextPtr> &cxts, const std::vector<jrun::vmachine::JObjectPtr> & args) -> jrun::vmachine::JObjectPtr
    {
      jrun::vmachine::JObjectPtr o1 = args.front();
      jrun::vmachine::JObjectPtr o2 = args.back();
      jrun::vmachine::JLiterObjectPtr l1 = std::dynamic_pointer_cast<jrun::vmachine::JLiterObject>(o1);
      jrun::vmachine::JLiterObjectPtr l2 = std::dynamic_pointer_cast<jrun::vmachine::JLiterObject>(o2);
      if( !(l1.get() && l2.get()) ) return jrun::vmachine::nullObject;
      return l1->operator+(l2);
    };
    cb.beforeRun = [add](std::vector<jrun::vmachine::JRunContextPtr>& cxts, std::shared_ptr<jrun::generation::AST> d) -> bool
    {
      std::string name("add");
      jrun::wrapper::registerFun(cxts, add, name, 2);
      return true;
    };
    cb.afterRun = [](jrun::vmachine::JObjectPtr r)->bool
    {
      if(r == jrun::vmachine::nullObject ) return false;
      try{
	jrun::vmachine::JLiterObjectPtr re = std::dynamic_pointer_cast<jrun::vmachine::JLiterObject>(r);
	if(re.get())
	  std::cout << "final value is : " << re->value << std::endl;
	else
	  std::cerr << "not an Literal value!" << std::endl;
	return true;
      } catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
      };
      return true;
    };
    
    if( jrun::wrapper::execute(std::string(filename), cb) )
       std::cout << "Finally success!" << std::endl;
    
    return 0;
}