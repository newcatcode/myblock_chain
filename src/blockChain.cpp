#include "block.hpp"
#include "util.hpp"
#include "transaction.hpp"
#include "blockChain.hpp"

Blockchain::Blockchain() {
    // Create the genesis block
    Block genesis_block(0, {});
    chain.push_back(genesis_block);
}

void Blockchain::add_block(const Block& block) {
    chain.push_back(block);
}

Block Blockchain::get_latest_block() const {
    return chain.back();
}

bool Blockchain::is_chain_valid() const {
    for (size_t i = 1; i < chain.size(); ++i) {
        const Block& current_block = chain[i];
        const Block& previous_block = chain[i - 1];

        // Check if the current block's previous hash matches the previous block's hash
        if (current_block.get_previous_hash() != previous_block.get_hash()) {
            return false;
        }

        // Check if the transactions in the current block are valid
        if (!current_block.verify_transactions()) {
            return false;
        }
    }
    return true;
}