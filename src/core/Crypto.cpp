#include "chat/core/Crypto.h"

#include <openssl/evp.h>

#include <array>
#include <sstream>
#include <iomanip>
#include <random>

namespace crypto
{
std::string md5_hex(const std::string& data)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");

    if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }

    if (EVP_DigestUpdate(ctx, data.c_str(), data.size()) != 1)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("EVP_DigestUpdate failed");
    }

    std::vector<unsigned char> digest(EVP_MD_size(EVP_md5()));
    unsigned int len = 0;
    if (EVP_DigestFinal_ex(ctx, digest.data(), &len) != 1)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }

    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < len; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return oss.str();
}

std::string random_hex(std::size_t n)
{
    static std::random_device random_device;
    static std::mt19937_64 gen(random_device());
    static std::uniform_int_distribution<int> dist(0, 15);
    static const char* hex = "0123456789abcdef";

    std::string out;
    out.resize(n * 2);
    for (size_t i = 0; i < n * 2; i++)
    {
        out[i] = hex[dist(gen)];
    }
    return out;
}
}  // namespace crypto