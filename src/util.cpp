#include"util.hpp"
#include <sstream>

util::time_point util::string_to_time_point(const std::string& timestamp_str) {
    std::tm tm = {};
    std::istringstream ss{timestamp_str};
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail())
        throw std::runtime_error("Bad timestamp: " + timestamp_str);
    std::time_t tt = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(tt);
}

std::string util::time_point_to_string(const time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}