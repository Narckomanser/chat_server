#pragma once

#include <sqlite3.h>
#include <string>
#include <chat/infra/db/dao/IUserDao.h>

class UserRepository
{
public:
    explicit UserRepository(IUserDao& dao);

    bool Exists(const std::string& user);
    std::optional<UserRow> Get(const std::string& user);
    bool Insert(const UserRow& user);
    void EnsureSchema();

private:
    IUserDao& dao_;
};
