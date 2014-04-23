set(evSRCS
  visitor.cc
  calculator.cc
)

add_library(evaluate ${evSRCS})
set_target_properties(evaluate PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(evaluate vmachine)