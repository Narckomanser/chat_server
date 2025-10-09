#pragma once

#include <chat/infra/db/IUserRepository.h>
#include <sqlite3.h>
#include <string>

class UserRepository : public IUserRepository
{
public:
    explicit UserRepository(std::string db_file);
    ~UserRepository();

    bool is_exist(std::string_view username) override;
    std::optional<User> get_user(std::string_view username) override;
    bool insert_user(const User &user) override;

private:
    void init_schema();

private:
    sqlite3* db_{nullptr};
};