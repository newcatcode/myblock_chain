#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP 
#include <string>
#include <chrono>
#include "util.hpp"
// Transaction definition goes here
class Transaction {
public:
    using time_point = std::chrono::system_clock::time_point;

    Transaction() = default;
    
    Transaction(u_int64_t transaction_id, std::string sender_key, std::string recipient_key, double amount, time_point timestamp = std::chrono::system_clock::now());
    // Compute the hash of the transaction
    std::string compute_hash() const;
    // Sign the transaction with the sender's private key
    void sign_transaction(const std::vector<u_int8_t>& private_key);
    // Verify the transaction's signature (自验证，使用存储的公钥)
    bool verify_signature() const;
    // 获取发送方公钥
    const std::vector<u_int8_t>& get_sender_public_key() const;
    // Convert the transaction to a string representation
    std::string to_string() const;
    // 获取交易ID
    u_int64_t get_id() const;
    // 获取发送方和接收方
    const std::string& get_sender() const;
    const std::string& get_recipient() const;
    // 获取交易金额
    double get_amount() const;
private:
    u_int64_t _transaction_id;
    std::string _sender_key;
    std::string _recipient_key;
    double _amount;
    u_int64_t _nonce;
    time_point _timestamp;
    std::string _signature;                     // 签名（hex 字符串）
    std::vector<u_int8_t> _sender_public_key;    // 发送方公钥（用于验证）
};
#endif // TRANSACTION_HPP