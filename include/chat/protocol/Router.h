#pragma once

#include <memory>
#include <string>

class AuthService;

class Router
{
public:
    explicit Router(std::shared_ptr<AuthService> auth) : auth_(auth) {}

    std::string handle(const std::string& json_line, bool& authenticated, std::string& username);

private:
    std::shared_ptr<AuthService> auth_;
};