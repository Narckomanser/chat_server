#include "../../include/chat/core/Command.h"

#include <sstream>

Command parse_command(std::string_view line)
{
    Command command;
    if (line.empty() || line[0] != '/') return command;

    std::istringstream iss(std::string(line.substr(1)));
    std::string cmd;
    iss >> cmd;

    if (cmd == "nick")
        command.kind = commandKind::Nick;
    else if (cmd == "join")
        command.kind = commandKind::Join;
    else if (cmd == "leave")
        command.kind = commandKind::Leave;
    else if (cmd == "rooms")
        command.kind = commandKind::Rooms;
    else if (cmd == "msg")
        command.kind = commandKind::Msg;
    else if (cmd == "quit")
        command.kind = commandKind::Quit;
    else
        command.kind = commandKind::Unknown;

    std::string token;
    while (iss >> token)
        command.args.push_back(std::move(token));

    return command;
}