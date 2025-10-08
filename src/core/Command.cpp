#include <chat/core/Command.h>

#include <sstream>

#include <chat/core/Message.h>
#include <chat/core/Types.h>
#include <chat/server/Server.h>
#include <chat/server/Session.h>
#include <chat/server/Room.h>

static std::shared_ptr<Server> need_server(Session& session)
{
    if (auto server = session.get_server().lock()) return server;
    session.deliver("server unavailable");

    return {};
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

CommandRegistry::CommandRegistry(std::weak_ptr<Server> server) : server_(std::move(server)) {}

void CommandRegistry::register_cmd(const std::string& name, std::unique_ptr<ICommand> cmd)
{
    cmds_[name] = std::move(cmd);
}

bool CommandRegistry::dispatch(Session& session, const ParsedCommand& command)
{
    const char* key = nullptr;
    switch (command.kind)
    {

        case commandKind::Nick: key = "nick"; break;
        case commandKind::Join: key = "join"; break;
        case commandKind::Leave: key = "leave"; break;
        case commandKind::Rooms: key = "rooms"; break;
        case commandKind::Msg: key = "msg"; break;
        case commandKind::Quit: key = "quit"; break;
        default: break;
    }

    if (!key) return false;

    if (auto it_cmd = cmds_.find(key); it_cmd != cmds_.end())
    {
        it_cmd->second->execute(session, command.args);
        return true;
    }

    return false;
}

void NickCommand::execute(Session& session, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        session.deliver(format_error("usage: /nick <name>"));
        return;
    }

    auto parsed = Nick::parse(args[0]);
    if (!parsed)
    {
        session.deliver(format_error("invalid nick"));
        return;
    }

    auto server = need_server(session);
    if (!server) return;

    std::string reason;
    if (server->set_nick(session.shared_from_this(), *parsed, reason))
    {
        session.set_nick(*parsed);
        session.deliver(format_info("nick set to " + parsed->str()));
    }
    else
        session.deliver(format_error(reason));
}

void JoinCommand::execute(Session& session, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        session.deliver(format_error("usage: /nick <name>"));
        return;
    }

    auto parsed = RoomName::parse(args[0]);
    if (!parsed)
    {
        session.deliver(format_error("invalid room"));
        return;
    }

    auto server = need_server(session);
    if (!server) return;

    auto new_room = server->get_or_create_room(parsed->room_name_);

    if (auto old_room = session.get_room().lock()) old_room->leave(session.shared_from_this());

    session.set_room(new_room);
    new_room->join(session.shared_from_this());

    session.deliver(format_info("joined " + parsed->room_name_));
}

void LeaveCommand::execute(Session& session, const std::vector<std::string>& args)
{
    if (auto room = session.get_room().lock())
    {
        auto room_name = room->get_room_name();
        room->leave(session.shared_from_this());
        session.set_room({});

        session.deliver(format_info("left from " + room_name));
    }
    else
        session.deliver(format_error("not in room"));
}

void RoomsCommand::execute(Session& session, const std::vector<std::string>& args)
{
    auto server = need_server(session);
    if (!server) return;

    auto room_list = server->get_list_rooms_detailed();
    if (room_list.empty())
    {
        session.deliver(format_info("rooms <none>"));
        return;
    }
    for (auto const& [name, size] : room_list)
    {
        session.deliver(format_info("room " + name + " members= " + std::to_string(size)));
    }
}

void MsgCommand::execute(Session& session, const std::vector<std::string>& args)
{
    if (!session.allow_sending_now())
    {
        session.deliver(format_error("rate limit"));
        return;
    }

    if (args.size() < 2)
    {
        session.deliver(format_error("usage: /msg <nick> <text>"));
        return;
    }

    auto to_parsed = Nick::parse(args[0]);
    if (!to_parsed)
    {
        session.deliver(format_error("invalid nick"));
        return;
    }

    auto server = need_server(session);
    if (!server) return;

    if (auto dst = server->find_session_by_nick(*to_parsed))
    {
        dst->deliver(format_pm(session.get_nick().name_, args[1]));
        session.deliver(format_pm_echo(dst->get_nick().name_, args[1]));
    }
    else
        session.deliver(format_error("nick not found"));
}

void QuitCommand::execute(Session& session, const std::vector<std::string>& args)
{
    session.close();
}


std::unique_ptr<CommandRegistry> build_default_registry(std::weak_ptr<Server> server)
{
    auto reg = std::make_unique<CommandRegistry>(std::move(server));

    reg->register_cmd("nick", std::make_unique<NickCommand>());
    reg->register_cmd("join", std::make_unique<JoinCommand>());
    reg->register_cmd("leave", std::make_unique<LeaveCommand>());
    reg->register_cmd("rooms", std::make_unique<RoomsCommand>());
    reg->register_cmd("msg", std::make_unique<MsgCommand>());
    reg->register_cmd("quit", std::make_unique<QuitCommand>());

    return reg;
}
