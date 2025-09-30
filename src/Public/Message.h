#pragma once

#include <string>

inline std::string format_info(const std::string& text)
{
    return ":server INFO " + text + "\n";
}

inline std::string format_error(const std::string& text)
{
    return ":server ERROR " + text + "\n";
}

inline std::string format_public(const std::string& room, const std::string& nick, const std::string& text)
{
    return room + "/" + nick + ": " + text + "\n";
}

inline std::string format_pm(const std::string& from, const std::string& text)
{
    return "[PM]" + from + ": " + text + "\n";
}

inline std::string format_pm_echo(const std::string& to, const std::string& text)
{
    return "[PM]to " + to + ": " + text + "\n";
}