#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP
#include <vector>
#include "block.hpp"
// Blockchain definition goes here
class Blockchain{
public:
    Blockchain(std::string name = "UnnamedChain");
    // Add a block to the blockchain
    void add_block(const Block& block);
    // Get the latest block in the blockchain
    Block get_latest_block() const;
    // Validate the integrity of the blockchain
    bool is_chain_valid() const;
    // 获取最新区块的哈希值
    std::string get_latest_block_hash() const;
    // 获取区块链长度
    size_t get_chain_length() const;
    // 将区块链转换为字符串表示
    std::string to_string() const;
    // 用指定创世块重置链
    void reset_chain(const Block& genesis_block);
    // 从另一个链拷贝所有区块
    void copy_from(const Blockchain& other);
private:
    // List of blocks in the blockchain
    std::vector<Block> chain;
};
#endif // BLOCKCHAIN_HPP
