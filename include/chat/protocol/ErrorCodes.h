#pragma once

#include <string_view>

namespace err
{
inline constexpr std::string_view bad_json = "bad_json";
inline constexpr std::string_view usage = "usage";
inline constexpr std::string_view unknown_type = "unknown_type";
inline constexpr std::string_view unknown_alias = "unknown_alias";
inline constexpr std::string_view not_authenticated = "not_authenticated";
inline constexpr std::string_view unknown_user = "unknown_user";
inline constexpr std::string_view auth_failed = "auth_failed";
inline constexpr std::string_view not_in_room = "not_in_room";
inline constexpr std::string_view no_such_room = "no_such_room";
inline constexpr std::string_view rate_limited = "rate_limited";
}  // namespace err