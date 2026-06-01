#include "transaction.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
/// @brief Constructor for creating a new transaction
/// @param sender_key The public key of the sender
/// @param recipient_key The public key of the recipient
/// @param amount The amount to be transferred
/// @param timestamp The timestamp of the transaction (default is the current time)
Transaction::Transaction(u_int64_t transaction_id, std::string sender_key, std::string recipient_key, double amount, time_point timestamp)
    : _transaction_id(transaction_id), _sender_key(std::move(sender_key)), _recipient_key(std::move(recipient_key)), _amount(amount), _timestamp(timestamp) {}


/// @brief Computes the hash of the transaction
/// @return The hash of the transaction
std::string Transaction::compute_hash() const{
    std::stringstream ss;
    ss << _transaction_id << _sender_key << _recipient_key << _amount << std::chrono::duration_cast<std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
    std::string data = ss.str();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    std::stringstream hash_ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hash_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return hash_ss.str();
}

/// @brief Signs the transaction with sender's private key using ECDSA
void Transaction::sign_transaction(const std::vector<u_int8_t>& private_key) {
    // 从私钥派生公钥并存储
    _sender_public_key = util::generate_public_key(private_key);
    // 对交易哈希进行 ECDSA 签名
    std::string hash = compute_hash();
    std::vector<u_int8_t> sig = util::ecdsa_sign(private_key, hash);
    // 签名转为 hex 存储
    _signature = util::bytes_to_hex(sig);
}

/// @brief Verifies the transaction's signature using stored public key
bool Transaction::verify_signature() const {
    // Coinbase 交易（无发送方）无需验证
    if (_sender_key.empty() || _sender_public_key.empty()) return true;
    // 将 hex 签名转回字节
    std::vector<u_int8_t> sig = util::hex_to_bytes(_signature);
    // 用存储的公钥验证签名
    return util::ecdsa_verify(_sender_public_key, compute_hash(), sig);
}

const std::vector<u_int8_t>& Transaction::get_sender_public_key() const {
    return _sender_public_key;
}

/// @brief Converts the transaction to a string representation
/// @return A string representation of the transaction
std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << "Transaction " << _transaction_id << " from " << _sender_key << " to " << _recipient_key << " of amount " << _amount << " at time " << std::chrono::duration_cast
        <std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
    return ss.str();
}

u_int64_t Transaction::get_id() const {
    return _transaction_id;
}

const std::string& Transaction::get_sender() const {
    return _sender_key;
}

const std::string& Transaction::get_recipient() const {
    return _recipient_key;
}

double Transaction::get_amount() const {
    return _amount;
}
