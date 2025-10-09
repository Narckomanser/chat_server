#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <optional>
#include <mutex>

struct Item
{
    std::string nonce;
    std::chrono::steady_clock::time_point expired_at;
    bool used = false;
};

class NonceStore
{
public:
    explicit NonceStore(int ttl_seconds) : ttl_(ttl_seconds) {}

    void put(const std::string& user, const std::string& nonce);
    std::optional<std::string> consume(const std::string& user);

private:
    std::unordered_map<std::string, Item> map_;
    std::chrono::seconds ttl_;
    std::mutex m_;
};