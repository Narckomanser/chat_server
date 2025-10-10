#pragma once

#include <chat/infra/db/dao/IUserDao.h>
#include <chat/infra/db/IDatabase.h>

#include <memory>

class SqliteUserDao : public IUserDao
{
public:
    explicit SqliteUserDao(std::shared_ptr<IDatabase> db);

    void ensureSchema() override;
    bool exists(const std::string& username) override;
    std::optional<UserRow> get(const std::string& username) override;
    bool insert(const UserRow& user) override;

private:
    std::shared_ptr<IDatabase> db_;
};