#pragma once

#include <string_view>

#include "../Public/Types.h"

struct NickHash
{
    using is_transparent = void;

    size_t operator()(Nick const& n) const noexcept { return std::hash<std::string>{}(n.name_); }
    size_t operator()(std::string_view const& sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

struct NickEq
{
    using is_transparent = void;

    bool operator()(Nick const& a, Nick const& b) const noexcept { return a.name_ == b.name_; }
    bool operator()(Nick const& a, std::string_view b) const noexcept { return Nick::canonicalize(a.name_) == b; }
    bool operator()(std::string_view a, Nick const& b) const noexcept { return a == Nick::canonicalize(b.name_); }
};