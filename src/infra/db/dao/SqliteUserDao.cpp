#include <chat/infra/db/dao/SqliteUserDao.h>

SqliteUserDao::SqliteUserDao(std::shared_ptr<IDatabase> db) : db_(db) {}

void SqliteUserDao::ensureSchema()
{
    db_->exec("CREATE TABLE IF NOT EXISTS users("
              " username   TEXT PRIMARY KEY,"
              " ha1        TEXT NOT NULL,"
              " created_at INTEGER NOT NULL"
              ");");
}

bool SqliteUserDao::exists(const std::string& username)
{
    auto st = db_->prepare("SELECT 1 FROM users WHERE username=? LIMIT 1;");
    st->bind(1, username);
    bool row = st->step();
    st.reset();

    return row;
}

std::optional<UserRow> SqliteUserDao::get(const std::string& username)
{
    auto st = db_->prepare("SELECT username, ha1, created_at FROM users WHERE username=? LIMIT 1;");
    st->bind(1, username);
    if (!st->step())
    {
        st.reset();
        return std::nullopt;
    }

    UserRow user;
    user.username = st->column_text(0);
    user.ha1 = st->column_text(1);
    user.created_at = st->column_int64(2);

    st->reset();

    return user;
}

bool SqliteUserDao::insert(const UserRow& user)
{
    auto st = db_->prepare("INSERT OR REPLACE INTO users (username, ha1, created_at) VALUES(?, ?, ?);");
    st->bind(0, user.username);
    st->bind(1, user.ha1);
    st->bind(2, user.created_at);

    bool result = !st->step();
    st->reset();

    return result;
}
