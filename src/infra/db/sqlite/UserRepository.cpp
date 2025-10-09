#include <chat/infra/db/sqlite/UserRepository.h>

UserRepository::UserRepository(std::string db_file)
{
    if (sqlite3_open(db_file.c_str(), &db_) != SQLITE_OK)
    {
        throw std::runtime_error("sqlite3_open failed");
    }
    init_schema();
}

UserRepository::~UserRepository()
{
    if (db_)
    {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool UserRepository::is_exist(std::string_view username)
{
    const char* req = "SELECT 1 FROM users WHERE username=? LIMIT 1;";

    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, username.data(), -1, &st, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(st, 1, req, (int)username.size(), SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    bool exists = (rc == SQLITE_ROW);
    sqlite3_finalize(st);

    return exists;
}

std::optional<User> UserRepository::get_user(std::string_view username)
{
    const char* req = "SELECT username, ha1, created_at FROM users WHERE username=? LIMIT 1;";

    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, username.data(), -1, &st, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(st, 1, req, (int)username.size(), SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(st);
        return std::nullopt;
    }

    User user;
    user.username = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
    user.ha1 = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
    user.created_at = sqlite3_column_int64(st, 2);

    sqlite3_finalize(st);

    return user;
}

bool UserRepository::insert_user(const User& user)
{
    const char* req = "INSERT OR REPLACE INTO users (username, ha1, created_at) VALUES(?, ?, ?);";

    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, req, -1, &st, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(st, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, user.ha1.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 3, user.created_at);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);

    return rc == SQLITE_DONE;
}

void UserRepository::init_schema()
{
    const char* req = "CREATE TABLE IF NOT EXISTS users("
                      " username TEXT PRIMARY KEY,"
                      " ha1 TEXT NOT NULL,"
                      " created_at INTEGER NOT NULL"
                      ");";

    char* err = nullptr;
    if (sqlite3_exec(db_, req, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::string msg = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("init_schema: " + msg);
    }
}