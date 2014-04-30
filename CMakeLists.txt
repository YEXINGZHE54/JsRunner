cmake_minimum_required(VERSION 2.8)

project(test_thread)

set(CXX_FLAGS
 -g
 -DDEBUG=1
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 -std=c++11
 #-stdlib=libstdc++
 )

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(parser)
add_subdirectory(vmachine)
add_subdirectory(log)
#add_subdirectory(evaluate)
add_executable(test_thread main.cpp)
add_subdirectory(wrapper)
set_target_properties(test_thread PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(test_thread parser vmachine log wrap)

install(TARGETS test_thread RUNTIME DESTINATION bin)