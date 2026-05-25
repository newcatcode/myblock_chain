#ifndef BLOCK_HPP
#define BLOCK_HPP
#include <string>
#include <vector>
#include "transaction.hpp"
// Block definition goes here
class Block{
public:
    Block() = default;
    // Add a transaction to the block
    Block(u_int64_t index, std::vector<Transaction>&& transactions, std::string previousHash = "");
    // Get the hash of the block
    std::string get_hash() const;
    // Get the previous hash of the block
    std::string get_previous_hash() const;
    // Mine the block with a given difficulty
    bool mine_block(u_int32_t difficulty);
    // Convert the block to a string representation
    std::string to_string() const;
    // verify the transactions in the block
    bool verify_transactions() const;
private:
    // general index of the block
    u_int64_t _index;
    // transactiondata of the block
    const std::vector<Transaction> _transactions;
    // merkle root of the transactions in the block
    std::string _merkle_root;
    //timestamp of the block
    std::string _timestamp;
    // hash of the block
    std::string Hash;
    // hash of the previous block
    std::string PreviousHash;
    // proof of work
    u_int64_t _nNonce;
    // calculate the hash of the block
    std::string calculate_hash() const;
    // calculate the merkle root of the transactions in the block
    std::string calculate_merkle_root() const;
};
#endif // BLOCK_HPP