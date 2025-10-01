cmake_minimum_required(VERSION 3.16)
project(chat_server CXX)
set(CMAKE_CXX_STANDARD 17)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)

add_library(chat-core
        # src/core
        src/core/Command.cpp
        src/core/RateLimiter.cpp
        src/core/WriteQueue.cpp

        # src/server
        src/server/NickRegistry.cpp
        src/server/Room.cpp
        src/server/Session.cpp
        src/server/Server.cpp
)

add_library(chat::core ALIAS chat-core)

target_include_directories(chat-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_link_libraries(chat-core
        PUBLIC Boost::boost
)

add_subdirectory(apps/chat_server)
