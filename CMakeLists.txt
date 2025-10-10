cmake_minimum_required(VERSION 3.16)
project(chat_v2 CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

add_library(chat-core
        src/core/Crypto.cpp

        src/protocol/Aliases.cpp
        src/protocol/Router.cpp

        src/infra/repos/UserRepository.cpp
        src/infra/security/NonceStore.cpp
        src/infra/db/sqlite/SqliteDatabase.cpp
        src/infra/db/dao/SqliteUserDao.cpp

        src/chat/app/services/AuthService.cpp

        src/transport/Server.cpp
        src/transport/Session.cpp
)

target_include_directories(chat-core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(chat-core
        PUBLIC Boost::boost
        PUBLIC OpenSSL::SSL OpenSSL::Crypto
        PUBLIC SQLite::SQLite3
        PUBLIC nlohmann_json::nlohmann_json
)

add_executable(chat_server apps/chat_server/main.cpp)
target_link_libraries(chat_server PRIVATE chat-core)
