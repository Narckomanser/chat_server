#pragma once

#include <chat/infra/db/IDatabase.h>

#include <sqlite3.h>
#include <memory>
#include <string>

class SqliteStatement : public IStatement
{
public:
    explicit SqliteStatement(sqlite3* db, const std::string& sql);
    ~SqliteStatement();

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
public:
    explicit SqliteDatabase(const std::string& file);
    ~SqliteDatabase();

    void exec(const std::string& sql) override;
    std::unique_ptr<IStatement> prepare(const std::string& sql) override;
    void withTransaction(std::function<void()>& work) override;

    sqlite3* row() const { return db_; }

private:
    sqlite3* db_{nullptr};
};