#include <chat/infra/db/sqlite/SqliteDatabase.h>

#include <stdexcept>

static void throw_if(int rc, sqlite3* db, const char* where)
{
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        const char* msg = sqlite3_errmsg(db);
        throw std::runtime_error(std::string(where) + ": " + (msg ? msg : "unknown sqlite error"));
    }
}

/*SqliteStatement*/
SqliteStatement::SqliteStatement(sqlite3* db, const std::string& sql) : db_(db)
{
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt_, nullptr);
    if (rc != SQLITE_OK) throw_if(rc, db_, "sqlite3_prepare_v2");
}

SqliteStatement::~SqliteStatement()
{
    if (stmt_) sqlite3_finalize(stmt_);
}

void SqliteStatement::bind(int idx, const std::string& v)
{
    int rc = sqlite3_bind_text(stmt_, idx, v.c_str(), -1, SQLITE_TRANSIENT);
    throw_if(rc, db_, "sqlite3_bind_text");
}

void SqliteStatement::bind(int idx, int64_t v)
{
    int rc = sqlite3_bind_int64(stmt_, idx, v);
    throw_if(rc, db_, "sqlite3_bind_int64");
}

bool SqliteStatement::step()
{
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) return true;
    if (rc == SQLITE_DONE) return false;
    throw_if(rc, db_, "sqlite3_step");
    return false;
}

void SqliteStatement::reset()
{
    int rc = sqlite3_reset(stmt_);
    throw_if(rc, db_, "sqlite3_reset");
    sqlite3_clear_bindings(stmt_);
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
SqliteDatabase::SqliteDatabase(const std::string& file)
{
    int rc = sqlite3_open(file.c_str(), &db_);
    if (rc != SQLITE_OK) throw_if(rc, db_, "sqlite3_open");
    exec("PRAGMA journal_mode=WAL;");
    exec("PRAGMA foreign_keys=ON;");
    exec("PRAGMA busy_timeout=3000;");
}

SqliteDatabase::~SqliteDatabase()
{
    if (db_) sqlite3_close(db_);
    db_ = nullptr;
}

void SqliteDatabase::exec(const std::string& sql)
{
    char* err = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err);
        throw_if(rc, db_, "sqlite3_exec");
    }
}

std::unique_ptr<IStatement> SqliteDatabase::prepare(const std::string& sql)
{
    return std::make_unique<SqliteStatement>(db_, sql);
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
