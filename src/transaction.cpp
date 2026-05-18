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

/// @brief Signs the transaction with the sender's private key
/// @param private_key The private key of the sender

void Transaction::sign_transaction(const std::string& private_key) {
    // Placeholder for signing logic
    _signature = "signed_with_" + private_key; // This is just a placeholder
}

/// @brief Verifies the transaction's signature
/// @param signature The signature to verify
/// @return True if the signature is valid, false otherwise

bool Transaction::verify_signature(std::string signature) const {
    // Placeholder for signature verification logic
    return  _signature == signature; // This is just a placeholder
}

/// @brief Converts the transaction to a string representation
/// @return A string representation of the transaction
std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << "Transaction " << _transaction_id << " from " << _sender_key << " to " << _recipient_key << " of amount " << _amount << " at time " << std::chrono::duration_cast
        <std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
    return ss.str();
}
