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
static const int TXS_PER_BLOCK = 8;         // 每个区块总交易数（含 coinbase）
static const int REGS_PER_BLOCK = TXS_PER_BLOCK - 1;  // 每轮普通交易数
static const int DIFFICULTY = 3;
static const int TOTAL_ROUNDS = 3;
static const auto TX_INTERVAL = std::chrono::milliseconds(200);

int main() {
    std::vector<std::string> names = {"Alice", "Bob", "Charlie", "Dave"};

    // ========== 1. 创建4个节点 ==========
    std::cout << "========== 创建节点 ==========" << std::endl;
    std::vector<User> users;
    std::vector<std::vector<u_int8_t>> priv_keys; // 保存私钥用于签名

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

    // ========== 2. Alice 率先创建创世区块（含 coinbase 奖励 100） ==========
    std::cout << "\n========== 生成创世区块 ==========" << std::endl;
    std::cout << "Alice 创建创世区块 (coinbase 奖励 100)..." << std::endl;
    {
        Transaction coinbase_tx(0, "", "Alice", 100.0);
        std::vector<Transaction> genesis_txs;
        genesis_txs.push_back(std::move(coinbase_tx));
        Block genesis_block(0, std::move(genesis_txs), "");
        genesis_block.mine_block(DIFFICULTY);
        users[0].get_blockchain().reset_chain(genesis_block);
        users[0].get_account().add_balance(100.0);
        std::cout << "创世区块哈希: " << genesis_block.get_hash() << std::endl;
        std::cout << "Alice 余额: " << users[0].get_account().get_balance() << std::endl;
    }

    // ========== 3. 其他节点从 Alice 同步区块链 ==========
    std::cout << "\n其他节点从 Alice 同步区块链..." << std::endl;
    for (int i = 1; i < NUM_NODES; ++i) {
        users[i].get_blockchain().copy_from(users[0].get_blockchain());
        std::cout << names[i] << " 已同步 (余额: "
                  << users[i].get_account().get_balance() << ")" << std::endl;
    }

    // ========== 4. 挖矿协调器 + 启动节点线程 ==========
    MiningCoordinator coordinator;
    for (int i = 0; i < NUM_NODES; ++i) {
        users[i].start(i, DIFFICULTY, &coordinator);
    }

    // 随机数生成器
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> node_dist(0, NUM_NODES - 1);
    std::uniform_real_distribution<double> amount_dist(1.0, 50.0);

    u_int64_t next_tx_id = 100; // 从100开始，避开 coinbase 的 ID

    std::cout << "\n========== 开始区块链模拟 ==========" << std::endl;
    std::cout << "节点: " << NUM_NODES << ", 难度: " << DIFFICULTY
              << ", 每块 " << TXS_PER_BLOCK << " 笔 (1 coinbase + "
              << REGS_PER_BLOCK << " 普通), 共 " << TOTAL_ROUNDS << " 轮" << std::endl;

    for (int round = 0; round < TOTAL_ROUNDS; ++round) {
        std::cout << "\n========== 第 " << (round + 1) << " 轮 ==========" << std::endl;

        // 生成 REGS_PER_BLOCK 笔普通交易，每笔检查发送方余额
        int txs_generated = 0;
        int retries = 0;
        const int MAX_RETRIES = 100;
        while (txs_generated < REGS_PER_BLOCK) {
            std::this_thread::sleep_for(TX_INTERVAL);

            int sender = node_dist(rng);
            int recipient = node_dist(rng);
            while (recipient == sender) recipient = node_dist(rng);

            double amount = std::round(amount_dist(rng) * 100.0) / 100.0;

            // 检查发送方余额
            double sender_balance = users[sender].get_account().get_balance();
            if (sender_balance < amount) {
                if (sender_balance > 0.0 && retries < MAX_RETRIES / 2) {
                    // 余额不够但有钱：用全部余额
                    amount = sender_balance;
                } else {
                    retries++;
                    if (retries > MAX_RETRIES) {
                        std::cout << "  警告: 超过最大重试次数，跳过此轮交易生成" << std::endl;
                        break;
                    }
                    continue;
                }
            }

            // 创建交易并使用发送方的私钥签名
            Transaction tx(next_tx_id++, names[sender], names[recipient], amount);
            tx.sign_transaction(priv_keys[sender]);

            std::cout << "生成交易 #" << next_tx_id - 1 << ": "
                      << names[sender] << " -> " << names[recipient] << " "
                      << amount << " 单位 (余额: " << sender_balance << ")" << std::endl;

            // 广播到所有节点
            UserMessage msg{UserMsgType::ADD_TRANSACTION, tx, {}};
            for (auto& user : users) {
                user.send_message(msg);
            }
            txs_generated++;
        }

        // 所有节点交易池已满，启动并行挖矿
        std::cout << "\n>>> 交易池已满，启动并行挖矿..." << std::endl;
        coordinator.winner_found.store(false);

        for (auto& user : users) {
            user.send_message({UserMsgType::START_MINING, {}, {}});
        }

        // 等待胜出者（最先挖到的节点通过 coordinator 通知）
        {
            std::unique_lock<std::mutex> lock(coordinator.mtx);
            coordinator.cv.wait(lock, [&coordinator]() {
                return coordinator.winner_found.load();
            });
        }

        std::cout << "<<< 优胜节点: " << coordinator.winner_name
                  << " (Nonce: " << coordinator.winner_block->get_nonce() << ")"
                  << std::endl;

        // 广播胜出的区块给所有节点验证并上链
        std::cout << "  广播区块到所有节点进行验证..." << std::endl;
        UserMessage block_msg{UserMsgType::BLOCK_MINED, {}, *coordinator.winner_block};
        for (auto& user : users) {
            user.send_message(block_msg);
        }

        // 给节点时间处理验证和上链
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ========== 停止所有节点线程 ==========
    std::cout << "\n========== 停止节点 ==========" << std::endl;
    for (auto& user : users) {
        user.stop();
    }
    for (auto& user : users) {
        user.join();
    }

    // ========== 打印最终状态 ==========
    std::cout << "\n========== 最终区块链状态 ==========" << std::endl;
    for (int i = 0; i < NUM_NODES; ++i) {
        std::cout << "\n----- " << names[i] << " (余额: "
                  << users[i].get_account().get_balance()
                  << ", 链长: " << users[i].get_blockchain().get_chain_length()
                  << ") -----" << std::endl;
        std::cout << users[i].get_blockchain().to_string() << std::endl;
    }

    // ========== 链完整性验证 ==========
    std::cout << "\n========== 链完整性验证 ==========" << std::endl;
    for (int i = 0; i < NUM_NODES; ++i) {
        bool valid = users[i].get_blockchain().is_chain_valid();
        std::cout << "[" << names[i] << "] " << (valid ? "✓ 有效" : "✗ 无效") << std::endl;
    }

    std::cout << "\n模拟结束。" << std::endl;
    return 0;
}
