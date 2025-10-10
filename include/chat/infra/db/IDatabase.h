#pragma once

#include <functional>
#include <string>
#include <vector>

struct IStatement
{
    virtual ~IStatement() = default;

    virtual void bind(int idx, const std::string& v) = 0;
    virtual void bind(int idx, int64_t v) = 0;
    virtual bool step() = 0;
    virtual void reset() = 0;
    virtual std::string column_text(int idx) = 0;
    virtual int64_t column_int64(int idx) = 0;
};

struct IDatabase
{
    virtual ~IDatabase() = default;

    virtual void exec(const std::string& sql) = 0;
    virtual std::unique_ptr<IStatement> prepare(const std::string& sql) = 0;

    virtual void withTransaction(std::function<void()>& work) = 0;
};