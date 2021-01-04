project(SimpleExampleTest)
find_package(Threads)
add_executable(Simple_exp ${CMAKE_CURRENT_SOURCE_DIR}/SimpleExample.cpp)
target_link_libraries (Simple_exp ${CMAKE_THREAD_LIBS_INIT})
