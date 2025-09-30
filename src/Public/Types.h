#pragma once

#include <regex>
#include <string>
#include <utility>
#include <optional>

struct Nick
{
    std::string name_;

    static bool valid(std::string_view sv)
    {
        static const std::regex re("^[A-Za-z0-9_-]{2,20}$");

        return std::regex_match(sv.begin(), sv.end(), re);
    }

    static std::string canonicalize(std::string new_nick)
    {
        std::transform(new_nick.begin(), new_nick.end(), new_nick.begin(), [](unsigned char c) { return std::tolower(c); });
        return new_nick;
    }

    static std::optional<Nick> parse(std::string new_nick)
    {
        if (!valid(new_nick)) return std::nullopt;
        return Nick{std::move(new_nick)};
    }

    [[nodiscard]] std::string const& str() const { return name_; }
};

inline bool operator==(Nick const& a, Nick const& b) noexcept
{
    return a.name_ == b.name_;
}

inline bool operator!=(Nick const& a, Nick const& b) noexcept
{
    return a.name_ != b.name_;
}


struct RoomName
{
    std::string room_name;

    static std::optional<std::string> validate(std::string_view sv)
    {
        static const std::regex re("^[A-Za-z0-9_-]{2,20}$");
        if (!std::regex_match(sv.begin(), sv.end(), re)) return std::string("invalid room name");

        return std::nullopt;
    }

    static std::optional<RoomName> parse(std::string s)
    {
        if (auto error = validate(s)) return std::nullopt;
        return RoomName{std::move(s)};
    }
};