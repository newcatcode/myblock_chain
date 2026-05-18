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
    
    Transaction(std::string sender_key, std::string recipient_key, double amount, time_point timestamp = std::chrono::system_clock::now());
    // Compute the hash of the transaction
    std::string compute_hash() const;
    // Sign the transaction with the sender's private key
    void sign_transaction(const std::string& private_key);
    // Verify the transaction's signature
    bool verify_signature(std::string signature) const;
    // Convert the transaction to a string representation
    std::string to_string() const;
private:
    std::string _sender_key;
    std::string _recipient_key;
    double _amount;
    time_point _timestamp;
    std::string _signature;
};
#endif // TRANSACTION_HPP