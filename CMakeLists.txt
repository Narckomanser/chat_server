cmake_minimum_required(VERSION 3.16)
project(chat_server CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Boost REQUIRED COMPONENTS system)

add_executable(chat_server
        src/main.cpp
        src/Private/Session.cpp
        src/Public/Session.h
        src/Private/Server.cpp
        src/Public/Server.h
        src/Private/Server.cpp
        src/Private/Session.cpp)

target_include_directories(chat_server PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(chat_server PRIVATE ${Boost_LIBRARIES})
