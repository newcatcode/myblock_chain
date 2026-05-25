#ifndef UTIL_HPP
#define UTIL_HPP
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
// Utility functions for the blockchain
namespace util {
    using time_point = std::chrono::system_clock::time_point;
    // Function to make a timestamp string to a time_point
    time_point string_to_time_point(const std::string& timestamp_str);
    std::string time_point_to_string(const time_point& tp) ;
    // Functions to generate private key, public key and address
    std::vector<u_int8_t> generate_private_key();
    std::vector<u_int8_t> generate_public_key(const std::vector<u_int8_t>& private_key);
    std::string address_from_public_key(const std::vector<u_int8_t>& public_key);
}
#endif // UTIL_HPP