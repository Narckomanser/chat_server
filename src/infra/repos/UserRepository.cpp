#include "chat/infra/repos/UserRepository.h"

UserRepository::UserRepository(IUserDao& dao) : dao_(dao) {}

bool UserRepository::exists(const std::string& user)
{
    return dao_.exists(user);
}

std::optional<UserRow> UserRepository::get(const std::string& user)
{
    auto row = dao_.get(user);
    if (!row) return std::nullopt;

    return row;
}

bool UserRepository::insert(const UserRow& user)
{
    return dao_.insert(user);
}

void UserRepository::EnsureSchema()
{
    dao_.ensureSchema();
}