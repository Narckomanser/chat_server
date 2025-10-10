#pragma once

#include <sqlite3.h>
#include <string>
#include <chat/infra/db/dao/IUserDao.h>

class UserRepository
{
public:
    explicit UserRepository(IUserDao& dao);

    bool exists(const std::string& user);
    std::optional<UserRow> get(const std::string& user);
    bool insert(const UserRow& user);
    void EnsureSchema();

private:
    IUserDao& dao_;
};
