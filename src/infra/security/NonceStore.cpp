#include <chat/infra/security/NonceStore.h>

void NonceStore::put(const std::string& user, const std::string& nonce)
{
    std::lock_guard<std::mutex> lg(m_);

    map_[user] = Item{nonce, std::chrono::steady_clock::now(), false};
}

std::optional<std::string> NonceStore::consume(const std::string& user)
{
    std::lock_guard<std::mutex> lg(m_);

    auto it = map_.find(user);
    if (it == map_.end()) return std::nullopt;

    auto& item = it->second;
    if (item.used || std::chrono::steady_clock::now() > item.expired_at)
    {
        map_.erase(it);
        return std::nullopt;
    }

    item.used = true;
    return item.nonce;
}