cmake_minimum_required(VERSION 3.6)
project(WilliamNet)
message("now_pos_is: "${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

#include(CheckCXXCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
## 使用变量设置编译标志
#if(COMPILER_SUPPORTS_CXX11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#elseif(COMPILER_SUPPORTS_CXX0X)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-1.18.1/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/NetCommon)


add_subdirectory(${CMAKE_SOURCE_DIR}/SimpleExample)
add_subdirectory(${CMAKE_SOURCE_DIR}/NetClient)
add_subdirectory(${CMAKE_SOURCE_DIR}/NetServer)


# add_subdirectory(${CMAKE_SOURCE_DIR}/NetCommon)
#add thread support
# find_package(Threads)
# target_link_libraries (Simple_exp ${CMAKE_THREAD_LIBS_INIT})
#