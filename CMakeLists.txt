cmake_minimum_required(VERSION 3.16)

# set the project name
project(EventMachine)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")

add_subdirectory(app)

# add the executable
add_executable(EventMachine main.cpp)

target_link_libraries(EventMachine
    PUBLIC
        EmCoreLib
)
