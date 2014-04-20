#include "log/logger.hpp"
#include <fstream>
#include <iostream>

using namespace jrun::log;

void Logger::init(const std::string& fname)
{
  std::ofstream log(fname);
  if(!log.is_open())
  {
    std::cerr << "filename error" << std::endl;
    return;
  }
  //Logger::logname = fname;
}

void Logger::log(level l, const std::string& msg)
{
  const char* levelChar;
  switch(l)
  {
    case level::INFO:
      levelChar = "LogInfo";
      break;
    case level::WARN:
      levelChar = "LogWarning";
      break;
    case level::ERRO:
      levelChar = "LogError";
      break;
    default:
      levelChar = "UnknownLogLevel";
      break;
  }
  std::cerr << levelChar << " " << msg << "\n";
}
