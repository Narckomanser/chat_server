cmake_minimum_required(VERSION 3.16)
project(chat_server CXX)
set(CMAKE_CXX_STANDARD 17)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)

add_library(chat-core)

add_executable(chat_server
        main.cpp
        ../../src/server/Session.cpp ../../include/chat/server/Session.h
        ../../src/server/Server.cpp ../../include/chat/server/Server.h
        ../../src/server/Room.cpp ../../include/chat/server/Room.h
        ../../include/chat/core/Log.h
        ../../include/chat/core/Types.h
        ../../src/core/Command.cpp ../../include/chat/core/Command.h

        ../../include/chat/core/Message.h
        ../../include/chat/core/WriteQueue.h ../../src/core/WriteQueue.cpp
        ../../include/chat/core/RateLimiter.h
        ../../src/core/RateLimiter.cpp
        ../../src/server/NickRegistry.cpp
        ../../include/chat/server/NickRegistry.h
        ../../include/chat/server/NickHash.h
)

target_include_directories(chat_server PRIVATE ${Boost_INCLUDE_DIRS})

target_compile_definitions(chat_server PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)
