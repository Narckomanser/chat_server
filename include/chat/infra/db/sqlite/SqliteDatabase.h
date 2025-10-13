#pragma once

#include <chat/infra/db/IDatabase.h>

#include <sqlite3.h>
#include <memory>
#include <string>
#include <functional>
#include <cstdint>

struct SqliteConfig
{
    bool enable_foreign_keys = false;
    bool wal_journal = true;
    int busy_timeout_ms = 3000;
};

class SqliteStatement : public IStatement
{
public:
    SqliteStatement(sqlite3* db, sqlite3_stmt* stmt) noexcept;
    ~SqliteStatement() override;

    SqliteStatement(SqliteStatement&) = delete;
    SqliteStatement(SqliteStatement&&) = delete;
    SqliteStatement& operator=(const SqliteStatement&) = delete;
    SqliteStatement& operator=(const SqliteStatement&&) = delete;

    void bind(int idx, int64_t v) override;
    void bind(int idx, const std::string& v) override;
    bool step() override;
    void reset() override;
    std::string column_text(int idx) override;
    int64_t column_int64(int idx) override;

private:
    sqlite3* db_{nullptr};
    sqlite3_stmt* stmt_{nullptr};
};

class SqliteDatabase : public IDatabase
{
private:
    SqliteDatabase() = default;

public:
    ~SqliteDatabase() override;

    SqliteDatabase(SqliteDatabase&) = delete;
    SqliteDatabase(SqliteDatabase&&) = delete;
    SqliteDatabase& operator=(const SqliteDatabase&) = delete;
    SqliteDatabase& operator=(const SqliteDatabase&&) = delete;

    static std::shared_ptr<SqliteDatabase> open(const std::string& path, const SqliteConfig& conf = {});

    void exec(const std::string& sql) override;
    std::unique_ptr<IStatement> prepare(const std::string& sql) override;
    void withTransaction(std::function<void()>& work) override;

    sqlite3* row() const noexcept { return db_; }
    const std::string& path() const noexcept { return path_; };

private:
    void open_impl(const std::string& path);
    void apply_config(const SqliteConfig& conf);

private:
    sqlite3* db_{nullptr};
    std::string path_;
};