cmake_minimum_required(VERSION 3.16)
project(chat_server CXX)
set(CMAKE_CXX_STANDARD 17)

set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(chat_core
        # src/core
        src/core/Command.cpp
        src/core/RateLimiter.cpp
        src/core/WriteQueue.cpp
        src/core/Crypto.cpp

        # src/server
        src/server/NickRegistry.cpp
        src/server/Room.cpp
        src/server/Session.cpp
        src/server/Server.cpp
)

add_library(chat::core ALIAS chat_core)

target_include_directories(chat_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_link_libraries(chat_core
        PUBLIC Boost::boost sqlite3 OpenSSL::Crypto
)

add_subdirectory(apps/chat_server)
