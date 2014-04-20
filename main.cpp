#include <iostream>
#include <fstream>
#include "parser/interface.hpp"
#include "vmachine/vmachine.hpp"
#include "log/logger.hpp"

using namespace std;
using namespace boost;

  void f(ifstream& in) {
      jrun::parser::genPtr data(new jrun::parser::genType());
      string str;
      in.unsetf(std::ios::skipws); // No white space skipping!
      std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(str));
      //while(std::getline(in, str)){
	char const* begin = str.c_str();
	char const* end = &begin[str.size()];  

	bool r  = jrun::parser::parse(begin, end, data);
	std::cout << "testing:" << str << std::endl;
	if(r) {
	  std::cout << "success!" << std::endl;
	  jrun::vmachine::VM::execute(data);
	} else {
	  std::cout << begin << std::endl;
	} 
    }

int main(int argc, char **argv) {
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

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }
    
    jrun::log::Logger::init("JRun.log");

    f(in);
    return 0;
}