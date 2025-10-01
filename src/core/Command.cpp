#include <chat/core/Command.h>

#include <sstream>

#include <chat/core/Message.h>
#include <chat/core/Types.h>
#include <chat/server/Server.h>
#include <chat/server/Session.h>
#include <chat/server/Room.h>

static std::shared_ptr<Server> need_server(Session& session)
{
    if (auto server = session.)
}


ParsedCommand parse_command(std::string_view line)
{
    ParsedCommand parsed_cmd;
    if (line.empty() || line[0] != '/') return parsed_cmd;

    std::istringstream iss(std::string(line.substr(1)));
    std::string cmd;
    iss >> cmd;

    if (cmd == "nick")
        parsed_cmd.kind = commandKind::Nick;
    else if (cmd == "join")
        parsed_cmd.kind = commandKind::Join;
    else if (cmd == "leave")
        parsed_cmd.kind = commandKind::Leave;
    else if (cmd == "rooms")
        parsed_cmd.kind = commandKind::Rooms;
    else if (cmd == "msg")
        parsed_cmd.kind = commandKind::Msg;
    else if (cmd == "quit")
        parsed_cmd.kind = commandKind::Quit;
    else
        parsed_cmd.kind = commandKind::Unknown;

    if (parsed_cmd.kind == commandKind::Msg)
    {
        std::string to;
        iss >> to;
        std::string text;
        std::getline(iss, text);

        if (!text.empty() && text[0] == ' ') text.erase(0, 1);
        if (!to.empty()) parsed_cmd.args.push_back(std::move(to));
        if (!text.empty()) parsed_cmd.args.push_back(std::move(text));

        return parsed_cmd;
    }

    std::string token;
    while (iss >> token)
        parsed_cmd.args.push_back(std::move(token));

    return parsed_cmd;
}

CommandRegistry::CommandRegistry(std::weak_ptr<Server> server)
{

}

void CommandRegistry::register_cmd(const std::string& name, std::unique_ptr<ICommand> cmd)
{

}

bool CommandRegistry::dispatch(Session& sessin, const ParsedCommand& command)
{

}



void NickCommand::execute(Session& session, const std::vector<std::string>& args)
{

}

void JoinCommand::execute(Session& session, const std::vector<std::string>& args)
{

}

void LeaveCommand::execute(Session& session, const std::vector<std::string>& args)
{

}

void RoomsCommand::execute(Session& session, const std::vector<std::string>& args)
{

}

void MsgCommand::execute(Session& session, const std::vector<std::string>& args)
{

}

void QuitCommand::execute(Session& session, const std::vector<std::string>& args)
{

}
