
project(SimpleClient)
find_package(Threads)
add_executable(SimpleClient ${CMAKE_CURRENT_SOURCE_DIR}/SimpleClient.cpp)
target_link_libraries (SimpleClient ${CMAKE_THREAD_LIBS_INIT})