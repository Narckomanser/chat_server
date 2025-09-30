#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

inline void log_line(const char* level, const char* comp, const std::string& message)
{
    using namespace std::chrono;

    auto now = system_clock::to_time_t(system_clock::now());
    auto gmt = *std::gmtime(&now);

    std::ostringstream ts;
    ts << std::put_time(&gmt, "%FT%TZ");

    std::cout << "[" << ts.str() << "]" << level << " " << comp << " " << message << std::endl;
}
