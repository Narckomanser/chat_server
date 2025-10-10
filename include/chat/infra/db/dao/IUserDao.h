#pragma once

#include <optional>
#include <string>
#include <cstdint>

struct UserRow
{
    std::string username;
    std::string ha1;
    int64_t created_at;
};

struct IUserDao
{
    virtual ~IUserDao() = default;

    virtual void ensureSchema() = 0;
    virtual bool exists(const std::string& username) = 0;
    virtual std::optional<UserRow> get(const std::string& username) = 0;
    virtual bool insert(const UserRow& user) = 0;
};