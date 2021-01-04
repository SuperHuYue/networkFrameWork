project(SimpleServer)
find_package(Threads)
add_executable(SimpleServer ${CMAKE_CURRENT_SOURCE_DIR}/SimpleServer.cpp)
target_link_libraries (SimpleServer ${CMAKE_THREAD_LIBS_INIT})