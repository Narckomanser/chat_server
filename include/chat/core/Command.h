#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <unordered_map>

class Server;
class Session;

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

struct ParsedCommand
{
    commandKind kind{commandKind::Unknown};
    std::vector<std::string> args;
};

ParsedCommand parse_command(std::string_view line);


struct ICommand
{
    virtual ~ICommand() = default;
    virtual void execute(Session& session, const std::vector<std::string>& args) = 0;
};


class CommandRegistry
{
public:
    explicit CommandRegistry(std::weak_ptr<Server> server);

    void register_cmd(const std::string& name, std::unique_ptr<ICommand> cmd);
    bool dispatch(Session& session, const ParsedCommand& command);

    std::shared_ptr<Server> get_server() { return server_.lock(); }

private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> cmds_;
    std::weak_ptr<Server> server_;
};

std::unique_ptr<CommandRegistry> build_default_registry(std::weak_ptr<Server> server);


struct NickCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};

struct JoinCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};

struct LeaveCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};

struct RoomsCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};

struct MsgCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};

struct QuitCommand : ICommand
{
    void execute(Session& session, const std::vector<std::string>& args) override;
};
