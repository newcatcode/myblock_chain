#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include "util.hpp"
#include "account.hpp"
#include "user.hpp"
#include "blockChain.hpp"
#include "transaction.hpp"

static const int NUM_NODES = 4;
static const int TXS_PER_BLOCK = 8;
static const int DIFFICULTY = 3;
static const int TOTAL_ROUNDS = 3;
static const auto TX_INTERVAL = std::chrono::milliseconds(200);

int main() {
    std::vector<std::string> names = {"Alice", "Bob", "Charlie", "Dave"};

    // 创建4个节点，保存私钥用于 ECDSA 签名
    std::cout << "========== 创建节点 ==========" << std::endl;
    std::vector<User> users;
    std::vector<std::vector<u_int8_t>> priv_keys;
    for (int i = 0; i < NUM_NODES; ++i) {
        auto priv = util::generate_private_key();
        auto pub = util::generate_public_key(priv);
        priv_keys.push_back(priv);
        Account acct(names[i], priv, pub);
        users.emplace_back(std::move(acct));
        std::cout << names[i] << " 地址: "
                  << util::address_from_public_key(users[i].get_account().get_public_key())
                  << std::endl;
    }

    // 挖矿协调器
    MiningCoordinator coordinator;

    // 启动所有节点线程
    for (int i = 0; i < NUM_NODES; ++i) {
        users[i].start(i, DIFFICULTY, &coordinator);
    }

    // 随机数生成器
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> node_dist(0, NUM_NODES - 1);
    std::uniform_real_distribution<double> amount_dist(1.0, 50.0);

    u_int64_t next_tx_id = 0;

    std::cout << "\n========== 开始区块链模拟 ==========" << std::endl;
    std::cout << "节点: " << NUM_NODES << ", 难度: " << DIFFICULTY
              << ", 每块交易数: " << TXS_PER_BLOCK
              << ", 轮数: " << TOTAL_ROUNDS << std::endl;

    for (int round = 0; round < TOTAL_ROUNDS; ++round) {
        std::cout << "\n========== 第 " << (round + 1) << " 轮 ==========" << std::endl;

        // 逐笔生成随机交易，使用 ECDSA 签名
        for (int t = 0; t < TXS_PER_BLOCK; ++t) {
            std::this_thread::sleep_for(TX_INTERVAL);

            int sender = node_dist(rng);
            int recipient = node_dist(rng);
            while (recipient == sender) recipient = node_dist(rng);

            double amount = std::round(amount_dist(rng) * 100.0) / 100.0;

            // ① 构造交易数据
            Transaction tx(next_tx_id++, names[sender], names[recipient], amount);
            // ② 用发送方私钥进行 ECDSA 签名（内部生成交易摘要并签名）
            tx.sign_transaction(priv_keys[sender]);
            // ③ 交易中已包含签名和发送方公钥，广播到所有节点

            std::cout << "生成交易 #" << next_tx_id - 1 << ": "
                      << names[sender] << " -> " << names[recipient] << " "
                      << amount << " 单位" << std::endl;

            // 广播到所有节点（各节点独立验证签名）
            UserMessage msg{UserMsgType::ADD_TRANSACTION, tx, {}};
            for (auto& user : users) {
                user.send_message(msg);
            }
        }

        // 开始挖矿
        std::cout << "\n>>> 交易池已满，启动并行挖矿..." << std::endl;
        coordinator.winner_found.store(false);

        for (auto& user : users) {
            user.send_message({UserMsgType::START_MINING, {}, {}});
        }

        // 等待胜出者
        {
            std::unique_lock<std::mutex> lock(coordinator.mtx);
            coordinator.cv.wait(lock, [&coordinator]() {
                return coordinator.winner_found.load();
            });
        }

        std::cout << "<<< 优胜节点: " << coordinator.winner_name
                  << " (Nonce: " << coordinator.winner_block->get_nonce() << ")"
                  << std::endl;

        // 广播区块到所有节点（各节点独立验证区块内所有交易的签名）
        std::cout << "  广播区块到所有节点进行 ECDSA 签名验证..." << std::endl;
        UserMessage block_msg{UserMsgType::BLOCK_MINED, {}, *coordinator.winner_block};
        for (auto& user : users) {
            user.send_message(block_msg);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 停止所有节点
    std::cout << "\n========== 停止节点 ==========" << std::endl;
    for (auto& user : users) {
        user.stop();
    }
    for (auto& user : users) {
        user.join();
    }

    // 打印最终状态
    std::cout << "\n========== 最终区块链状态 ==========" << std::endl;
    for (int i = 0; i < NUM_NODES; ++i) {
        std::cout << "\n----- " << names[i] << " ("
                  << users[i].get_blockchain().get_chain_length() << " 个区块) -----" << std::endl;
        std::cout << users[i].get_blockchain().to_string() << std::endl;
    }

    // 验证所有链
    std::cout << "\n========== 链完整性验证 ==========" << std::endl;
    for (int i = 0; i < NUM_NODES; ++i) {
        bool valid = users[i].get_blockchain().is_chain_valid();
        std::cout << "[" << names[i] << "] " << (valid ? "✓ 有效" : "✗ 无效") << std::endl;
    }

    std::cout << "\n模拟结束。" << std::endl;
    return 0;
}
