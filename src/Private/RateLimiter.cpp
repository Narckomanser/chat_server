#include "../Public/RateLimiter.h"

bool RateLimiter::allow()
{
    using clock = std::chrono::steady_clock;
    using namespace std::chrono;

    auto now = clock::now();
    auto cutoff = now - 1s;

    while (!queue_.empty() && queue_.front() < cutoff)
        queue_.pop_front();

    if (queue_.size() >= max_) return false;

    queue_.push_back(now);
    return true;
}