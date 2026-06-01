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

std::string Blockchain::get_latest_block_hash() const {
    return chain.back().get_hash();
}

size_t Blockchain::get_chain_length() const {
    return chain.size();
}

std::string Blockchain::to_string() const {
    std::ostringstream ss;
    for (size_t i = 0; i < chain.size(); ++i) {
        ss << chain[i].to_string();
        if (i != chain.size() - 1) {
            ss << "-----------------------------\n";
        }
    }
    return ss.str();
}

void Blockchain::reset_chain(const Block& genesis_block) {
    chain.clear();
    chain.push_back(genesis_block);
}

void Blockchain::copy_from(const Blockchain& other) {
    chain.clear();
    chain.reserve(other.chain.size());
    for (const auto& b : other.chain) {
        chain.push_back(b);
    }
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