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
#include <openssl/ecdsa.h>
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
    // ECDSA 签名与验证
    std::vector<u_int8_t> ecdsa_sign(const std::vector<u_int8_t>& private_key, const std::string& data);
    bool ecdsa_verify(const std::vector<u_int8_t>& public_key, const std::string& data, const std::vector<u_int8_t>& signature);
    // 十六进制转换工具
    std::string bytes_to_hex(const std::vector<u_int8_t>& bytes);
    std::vector<u_int8_t> hex_to_bytes(const std::string& hex);
}
#endif // UTIL_HPP