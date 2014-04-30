set(vmSRCS
  JObject.cc
  vmachine.cc
  JRunContext.cc
  closure.cc
  operation.cc
  ../evaluate/visitor.cc
  ../evaluate/calculator.cc
)

add_library(vmachine ${vmSRCS})
set_target_properties(vmachine PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(vmachine log)
