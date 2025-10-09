#pragma once

#include <optional>
#include <string>
#include <cstdint>

struct User
{
    std::string username;
    std::string ha1;
    int64_t created_at{};
};

struct IUserRepository
{
    virtual ~IUserRepository() = default;
    virtual bool is_exist(std::string_view username) = 0;
    virtual std::optional<User> get_user(std::string_view username) = 0;
    virtual bool insert_user(const User& user) = 0;
};