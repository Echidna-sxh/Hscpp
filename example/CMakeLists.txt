CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Tests)                                                                  

add_executable(test test.cpp)
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
link_directories(${CMAKE_INSTALL_PREFIX}/lib64)
target_link_libraries(test -lhs -lhscpp)