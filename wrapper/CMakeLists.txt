set(wrapSRCS
  wrapper.cc
)

add_library(wrap ${wrapSRCS})
set_target_properties(wrap PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(wrap parser vmachine)