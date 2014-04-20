set(parseSRCS
  interface.cc
)

add_library(parser ${parseSRCS})
set_target_properties(parser PROPERTIES COMPILE_FLAGS "-std=c++11")
