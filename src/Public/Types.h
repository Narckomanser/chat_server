#pragma once

#include <regex>
#include <string>
#include <utility>
#include <optional>

struct Nick
{
    std::string nick;

    static std::optional<std::string> validate(std::string_view sv)
    {
        static const std::regex re("^[A-Za-z0-9_-]{2,20}$");
        if (!std::regex_match(sv.begin(), sv.end(), re)) return std::string("invalid nick");

        return std::nullopt;
    }

    static std::optional<Nick> parse(std::string s)
    {
        if (auto error = validate(s)) return std::nullopt;
        return Nick{std::move(s)};
    }
};

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