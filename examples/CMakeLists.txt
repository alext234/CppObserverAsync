project(cppobserver_example)

include_directories(${CMAKE_SOURCE_DIR})
add_executable(async_observers  async_observers.cpp)
target_link_libraries(async_observers pthread)
