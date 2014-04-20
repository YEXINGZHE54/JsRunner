#ifndef JRUN_LOG_HPP
#define JRUN_LOG_HPP

#include <string>
#include <memory>
#include <vector>
#include "log/Buffer.h"

namespace jrun{
  namespace log{
      enum level{
	INFO,
	WARN,
	ERRO
      };
    typedef std::shared_ptr<Buffer> bufPtr;
    class Logger{      
    public:
      static void init(const std::string& fname);
      static void log(level l, const std::string& msg);     
      static  std::vector< bufPtr > Buffers;
      static std::string logname;
    };
  }
}

#endif