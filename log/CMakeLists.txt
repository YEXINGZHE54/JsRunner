set(logSRCS
  logger.cc
)

add_library(log ${logSRCS})
set_target_properties(log PROPERTIES COMPILE_FLAGS "-std=c++11")
