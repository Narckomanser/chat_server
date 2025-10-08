#pragma once

#include <string>

namespace crypto
{

std::string md5_hex(const std::string& data);
std::string random_hex(std::size_t n = 16);

};
