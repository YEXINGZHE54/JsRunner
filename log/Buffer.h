#ifndef JRUN_LOG_BUFFER_HPP
#define JRUN_LOG_BUFFER_HPP

#include <string>

namespace jrun{
  namespace log{
    class Buffer{
    public:
      const static int BUFFER_MAX_SIZE = 1024;
      void append(std::string msg);
    private:
      char buf[BUFFER_MAX_SIZE];
    };
  }
}

#endif