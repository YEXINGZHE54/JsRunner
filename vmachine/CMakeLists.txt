set(vmSRCS
  JObject.cc
  operation.cc
  visitor.cc
  vmachine.cc
  JRunContext.cc
)

add_library(vmachine ${vmSRCS})
set_target_properties(vmachine PROPERTIES COMPILE_FLAGS "-std=c++11")
