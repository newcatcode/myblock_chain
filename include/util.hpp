#ifndef UTIL_HPP
#define UTIL_HPP
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
// Utility functions for the blockchain
namespace util {
    using time_point = std::chrono::system_clock::time_point;
    // Function to make a timestamp string to a time_point
    time_point string_to_time_point(const std::string& timestamp_str);
    std::string time_point_to_string(const time_point& tp) ;
}
#endif // UTIL_HPP