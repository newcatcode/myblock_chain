#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP
#include <vector>
#include "block.hpp"
// Blockchain definition goes here
class Blockchain{
public:
    Blockchain();
    // Add a block to the blockchain
    void add_block(const Block& block);
    // Get the latest block in the blockchain
    Block get_latest_block() const;
    // Validate the integrity of the blockchain
    bool is_chain_valid() const;
private:
    // List of blocks in the blockchain
    std::vector<Block> chain;
};
#endif // BLOCKCHAIN_HPP
