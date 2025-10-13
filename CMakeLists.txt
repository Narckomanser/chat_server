cmake_minimum_required(VERSION 3.16)
project(chat_v2 CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- Warnings ----
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- Dependencies ----
find_package(Boost REQUIRED)          # header-only asio ок
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ---- Library: chat-core ----
add_library(chat-core
        # core
        src/core/Crypto.cpp

        # protocol
        src/protocol/dto/Dto.cpp
        src/protocol/JsonCodec.cpp
        src/protocol/Aliases.cpp
        src/protocol/Router.cpp

        # protocol/session ctx

        # services
        src/chat/app/services/AuthService.cpp
        src/chat/app/services/RoomService.cpp

        # handlers
        src/chat/app/handlers/AuthHandlers.cpp
        src/chat/app/handlers/RoomHandlers.cpp
        src/chat/app/handlers/ChatHandlers.cpp

        # infra: db + security
        src/infra/db/sqlite/SqliteDatabase.cpp
        src/infra/db/dao/SqliteUserDao.cpp
        src/infra/security/NonceStore.cpp
        src/infra/repos/UserRepository.cpp

        # transport
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

# ---- Executable ----
add_executable(chat_server apps/chat_server/main.cpp)
target_link_libraries(chat_server PRIVATE chat-core)

# ---- (Optional) LTO ----
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_msg)
if(ipo_supported)
    set_property(TARGET chat-core PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET chat_server PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# ---- (Optional) Install ----
install(TARGETS chat_server RUNTIME DESTINATION bin)
