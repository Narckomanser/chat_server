#include "../Public/NickRegistry.h"

bool NickRegistry::set(const std::shared_ptr<Session>& who, const Nick& nick, std::string& reason)
{
    if (const auto it_nick = registry_.find(nick); it_nick != registry_.end())
    {
        if (const auto alive = it_nick->second.lock())
        {
            if (alive.get() != who.get())
            {
                reason = "nick already taken";
                return false;
            }
            else registry_.erase(it_nick);
        }
    }

    registry_[nick] = who;
    return true;
}

std::shared_ptr<Session> NickRegistry::find(const std::string_view& nick_sv)
{
    Nick canonical_nick;
    canonical_nick.name_ = Nick::canonicalize(std::string(nick_sv));

    return find(canonical_nick);
}

std::shared_ptr<Session> NickRegistry::find(const Nick& nick)
{
    if (auto it_nick = registry_.find(nick); it_nick != registry_.end())
    {
        if (auto sp = it_nick->second.lock()) return sp;
        registry_.erase(it_nick);
    }

    return {};
}