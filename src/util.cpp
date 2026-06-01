#include"util.hpp"

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

std::vector<u_int8_t> util::generate_private_key() {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!key) throw std::runtime_error("EC_KEY_new_by_curve_name failed");

    if (!EC_KEY_generate_key(key)) {
        EC_KEY_free(key);
        throw std::runtime_error("EC_KEY_generate_key failed");
    }

    const BIGNUM* priv_bn = EC_KEY_get0_private_key(key);
    std::vector<uint8_t> priv_bytes(32);
    BN_bn2binpad(priv_bn, priv_bytes.data(), 32);

    EC_KEY_free(key);
    return priv_bytes;
}

std::vector<u_int8_t> util::generate_public_key(const std::vector<u_int8_t>& private_key) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    BIGNUM* priv_bn = BN_bin2bn(private_key.data(), private_key.size(), nullptr);

    EC_KEY_set_private_key(key, priv_bn);

    const EC_GROUP* group = EC_KEY_get0_group(key);
    EC_POINT* pub_point = EC_POINT_new(group);
    EC_POINT_mul(group, pub_point, priv_bn, nullptr, nullptr, nullptr);

    std::vector<uint8_t> pub_bytes(65);
    EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_UNCOMPRESSED,
                       pub_bytes.data(), 65, nullptr);

    BN_free(priv_bn);
    EC_POINT_free(pub_point);
    EC_KEY_free(key);
    return pub_bytes;
}

std::vector<u_int8_t> util::ecdsa_sign(const std::vector<u_int8_t>& private_key, const std::string& data) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    BIGNUM* priv_bn = BN_bin2bn(private_key.data(), private_key.size(), nullptr);
    EC_KEY_set_private_key(key, priv_bn);

    const EC_GROUP* group = EC_KEY_get0_group(key);
    EC_POINT* pub_point = EC_POINT_new(group);
    EC_POINT_mul(group, pub_point, priv_bn, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(key, pub_point);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    unsigned char sig_buf[256];
    unsigned int sig_len = 0;
    ECDSA_sign(0, hash, SHA256_DIGEST_LENGTH, sig_buf, &sig_len, key);

    std::vector<u_int8_t> result(sig_buf, sig_buf + sig_len);

    EC_POINT_free(pub_point);
    BN_free(priv_bn);
    EC_KEY_free(key);
    return result;
}

bool util::ecdsa_verify(const std::vector<u_int8_t>& public_key, const std::string& data, const std::vector<u_int8_t>& signature) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    const EC_GROUP* group = EC_KEY_get0_group(key);
    EC_POINT* pub_point = EC_POINT_new(group);
    EC_POINT_oct2point(group, pub_point, public_key.data(), public_key.size(), nullptr);
    EC_KEY_set_public_key(key, pub_point);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    int ret = ECDSA_verify(0, hash, SHA256_DIGEST_LENGTH,
                           signature.data(), signature.size(), key);

    EC_POINT_free(pub_point);
    EC_KEY_free(key);
    return ret == 1;
}

std::string util::bytes_to_hex(const std::vector<u_int8_t>& bytes) {
    std::ostringstream ss;
    for (auto b : bytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return ss.str();
}

std::vector<u_int8_t> util::hex_to_bytes(const std::string& hex) {
    std::vector<u_int8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        bytes.push_back(static_cast<u_int8_t>(std::stoi(byte_str, nullptr, 16)));
    }
    return bytes;
}

std::string util::address_from_public_key(const std::vector<u_int8_t>& public_key) {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(public_key.data(), public_key.size(), hash);

    std::string address;
    for (int i = 0; i < 20; ++i) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        address += buf;
    }
    return address;
}