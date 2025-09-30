cmake_minimum_required(VERSION 3.16)
project(chat_server CXX)
set(CMAKE_CXX_STANDARD 17)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)

add_executable(chat_server
        src/main.cpp
        src/Private/Session.cpp     src/Public/Session.h
        src/Private/Server.cpp      src/Public/Server.h
        src/Private/Room.cpp        src/Public/Room.h
                                    src/Public/Log.h
                                    src/Public/Types.h
        src/Private/Command.cpp     src/Public/Command.h

                                    src/Public/Message.h
        src/Public/WriteQueue.h     src/Private/WriteQueue.cpp
        src/Public/RateLimiter.h
        src/Private/RateLimiter.cpp
)

target_include_directories(chat_server PRIVATE ${Boost_INCLUDE_DIRS})

target_compile_definitions(chat_server PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)
