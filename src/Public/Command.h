#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class commandKind
{
    Nick,
    Join,
    Leave,
    Rooms,
    Msg,
    Quit,
    Unknown
};

struct Command
{
    commandKind kind{commandKind::Unknown};
    std::vector<std::string> args;
};

Command parse_command(std::string_view line);