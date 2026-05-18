#include <iostream>
#include <vector>
#include <thread>
#include "blockChain.hpp"
#include "transaction.hpp"
int main() {
    const std::vector<std::string> userPublicKeys{"user1PublicKey", "user2PublicKey", "user3PublicKey", "user4PublicKey"};
    std::vector<Transaction> transactionPool;
    transactionPool.reserve(4);
    // Create a Transaction
    Transaction transaction1(1, "AlicePublicKey", "BobPublicKey", 10.0);
    Transaction transaction2(2, "BobPublicKey", "CharliePublicKey", 5.0);
    Transaction transaction3(3, "CharliePublicKey", "DavePublicKey", 2.5);
    Transaction transaction4(4, "DavePublicKey", "AlicePublicKey", 1.0);
    // Add transactions to the transaction pool
    transactionPool.push_back(transaction1);
    transactionPool.push_back(transaction2);
    transactionPool.push_back(transaction3);
    transactionPool.push_back(transaction4);
    // Create a blockchain and add a block with the transactions
    Blockchain blockchain;
    Block block(1, std::move(transactionPool));
    blockchain.add_block(block);
    
}