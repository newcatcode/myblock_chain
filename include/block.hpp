#ifndef BLOCK_HPP
#define BLOCK_HPP
#include <string>
#include <vector>
#include "transactionList.hpp"
// Block definition goes here
class Block{
public:
    Block() = default;
    // Add a transaction to the block
    Block(u_int32_t index,TransactionList& transactions, std::string previousHash = "");
    // Get the hash of the block
    std::string get_hash() const;
    // Get the previous hash of the block
    std::string get_previous_hash() const;
    // Mine the block with a given difficulty
    void mine_block(u_int32_t difficulty);
    // Convert the block to a string representation
    std::string to_string() const;
private:
    // general index of the block
    u_int32_t _index;
    // transactiondata of the block
    std::string _transactions;
    //timestamp of the block
    std::string _timestamp;
    // hash of the block
    std::string Hash;
    // hash of the previous block
    std::string PreviousHash;
    // proof of work
    std::string _nNonce;
    // calculate the hash of the block
    std::string calculate_hash() const;

};
#endif // BLOCK_HPP