#pragma once

#include <deque>
#include <chrono>

class RateLimiter
{
public:
    explicit RateLimiter(std::size_t max = 10) : max_(max) {}

    bool allow();

private:
    std::deque<std::chrono::steady_clock::time_point> queue_;
    size_t max_;
};