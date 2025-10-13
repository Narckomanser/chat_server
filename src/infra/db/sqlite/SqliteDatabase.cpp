#include <chat/infra/db/sqlite/SqliteDatabase.h>

#include <stdexcept>
#include <string>

namespace
{
[[noreturn]] void throw_sqlite(sqlite3* db, const char* where)
{
    const char* msg = sqlite3_errmsg(db);
    throw std::runtime_error(std::string(where) + ": " + (msg ? msg : "unknown sqlite error"));
}

inline void check_ok(int rc, sqlite3* db, const char* where)
{
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        throw_sqlite(db, where);
    }
}
}  // namespace

/*SqliteStatement*/
SqliteStatement::SqliteStatement(sqlite3* db, sqlite3_stmt* stmt) noexcept : db_(db), stmt_(stmt) {}

SqliteStatement::~SqliteStatement()
{
    if (stmt_) sqlite3_finalize(stmt_);
}

void SqliteStatement::bind(int idx, const std::string& v)
{
    int rc = sqlite3_bind_text(stmt_, idx, v.c_str(), -1, SQLITE_TRANSIENT);
    check_ok(rc, db_, "sqlite3_bind_text");
}

void SqliteStatement::bind(int idx, int64_t v)
{
    int rc = sqlite3_bind_int64(stmt_, idx, v);
    check_ok(rc, db_, "sqlite3_bind_int64");
}

bool SqliteStatement::step()
{
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) return true;
    if (rc == SQLITE_DONE) return false;
    check_ok(rc, db_, "sqlite3_step");
    return false;
}

void SqliteStatement::reset()
{
    check_ok(sqlite3_reset(stmt_), db_, "sqlite3_reset");
    check_ok(sqlite3_clear_bindings(stmt_), db_, "sqlite3_clear_bindings");
}

std::string SqliteStatement::column_text(int idx)
{
    const unsigned char* text = sqlite3_column_text(stmt_, idx);
    return text ? reinterpret_cast<const char*>(text) : std::string{};
}

int64_t SqliteStatement::column_int64(int idx)
{
    return sqlite3_column_int64(stmt_, idx);
}

/*SqliteDatabase*/
std::shared_ptr<SqliteDatabase> SqliteDatabase::open(const std::string& path, const SqliteConfig& conf)
{
    auto db = std::shared_ptr<SqliteDatabase>(new SqliteDatabase());
    db->open_impl(path);
    db->apply_config(conf);

    return db;
}

void SqliteDatabase::open_impl(const std::string& path)
{
    const int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    sqlite3* tmp = nullptr;
    int rc = sqlite3_open_v2(path.c_str(), &tmp, flags, nullptr);
    if (rc != SQLITE_OK)
    {
        if (tmp) sqlite3_close(tmp);
        throw std::runtime_error("sqlite3_open_v2 failed: " + std::to_string(rc));
    }
    db_ = tmp;
    path_ = path;
}

void SqliteDatabase::apply_config(const SqliteConfig& cfg)
{
    if (sqlite3_busy_timeout(db_, cfg.busy_timeout_ms) != SQLITE_OK)
    {
        throw_sqlite(db_, "sqlite3_busy_timeout");
    }

    if (cfg.enable_foreign_keys)
    {
        exec("PRAGMA foreign_keys=ON;");
    }
    else
    {
        exec("PRAGMA foreign_keys=OFF;");
    }
    
    if (cfg.wal_journal)
    {
        exec("PRAGMA journal_mode=WAL;");
    }
    else
    {
        exec("PRAGMA journal_mode=DELETE;");
    }
}

SqliteDatabase::~SqliteDatabase()
{
    if (db_)
    {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void SqliteDatabase::exec(const std::string& sql)
{
    char* err = nullptr;
    const int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err);
        check_ok(rc, db_, "sqlite3_exec");
    }
}

std::unique_ptr<IStatement> SqliteDatabase::prepare(const std::string& sql)
{
    sqlite3_stmt* stmt = nullptr;
    const int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) throw_sqlite(db_, "sqlite3_prepare_v2");
    return std::make_unique<SqliteStatement>(db_, stmt);
}

void SqliteDatabase::withTransaction(std::function<void()>& work)
{
    exec("BEGIN IMMEDIATE;");
    try
    {
        work();
        exec("COMMIT;");
    }
    catch (...)
    {
        exec("ROLLBACK;");
        throw;
    }
}
