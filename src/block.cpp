# include "block.hpp"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "util.hpp"
#include "transaction.hpp"

Block::Block(u_int64_t index, 
             std::vector<Transaction>&& transactions,
             std::string previousHash)
    : _index(index)
    , _transactions(std::move(transactions))
    , PreviousHash(previousHash)
    , _merkle_root(calculate_merkle_root())
    , _timestamp(util::time_point_to_string(std::chrono::system_clock::now()))
{
}

// get the hash of the block
std::string Block::get_hash() const {
    return Hash;
}

// get the previous hash of the block
std::string Block::get_previous_hash() const {
    return PreviousHash;
}

// mine the block with a given difficulty
bool Block::mine_block(u_int32_t difficulty) {
    std::string str(difficulty, '0');
    while (Hash.substr(0, difficulty) != str) {
        _nNonce++;
        Hash = calculate_hash();
    }
    return true;
}

// convert the block to a string representation
std::string Block::to_string() const {
    std::ostringstream ss;
    ss << "Block #" << _index << "\n";
    ss << "Previous Hash: " << PreviousHash << "\n";
    ss << "Merkle Root: " << _merkle_root << "\n";
    ss << "Timestamp: " << _timestamp << "\n";
    ss << "Nonce: " << _nNonce << "\n";
    ss << "Hash: " << Hash << "\n";
    ss << "Transactions:\n";
    for (const auto& transaction : _transactions) {
        ss << transaction.to_string() << "\n";
    }
    return ss.str();
}   

// verify the transactions in the block
bool Block::verify_transactions() const {
    for (const auto& transaction : _transactions) {
        if (!transaction.verify_signature(transaction.compute_hash())) {
            return false;
        }
    }
    return true;
}

// calculate the hash of the block
std::string Block::calculate_hash() const {
    std::ostringstream ss;
    ss << _index << PreviousHash << _merkle_root << _timestamp << _nNonce;
    std::string blockData = ss.str();
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)blockData.c_str(), blockData.size(), hash);
    std::ostringstream hashString;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hashString << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return hashString.str();
}

// calculate the merkle root of the transactions in the block
std::string Block::calculate_merkle_root() const {
    if (_transactions.empty()) {
        return "";
    }
    std::vector<std::string> merkleTree;
    for (const auto& transaction : _transactions) {
        merkleTree.push_back(transaction.compute_hash());
    }
    while (merkleTree.size() > 1) {
        if (merkleTree.size() % 2 != 0) {
            merkleTree.push_back(merkleTree.back());
        }
        std::vector<std::string> newMerkleTree;
        for (size_t i = 0; i < merkleTree.size(); i += 2) {
            std::string combinedHash = merkleTree[i] + merkleTree[i + 1];
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256((unsigned char*)combinedHash.c_str(), combinedHash.size(), hash);
            std::ostringstream hashString;
            for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
                hashString << std::hex << std::setw(2) << std::setfill('0') << (int)hash[j];
            }
            newMerkleTree.push_back(hashString.str());
        }
        merkleTree = std::move(newMerkleTree);
    }
    return merkleTree.front();
}