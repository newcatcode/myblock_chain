#ifndef USER_HPP
#define USER_HPP
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <iostream>
#include "account.hpp"
#include "transaction.hpp"
#include "blockChain.hpp"
// 挖矿结果结构体
struct MiningResult {
    std::string node_name;
    u_int64_t nonce;
    double duration_ms;
    bool success;
};

// 用户消息类型
enum class UserMsgType {
    ADD_TRANSACTION,   ///< 添加交易到交易池
    START_MINING,      ///< 开始挖矿
    BLOCK_MINED,       ///< 有节点挖出区块，验证并上链
    STOP               ///< 停止线程
};

// 用户消息结构体
struct UserMessage {
    UserMsgType type;
    Transaction tx;
    Block block;
};

// 挖矿协调器（用于主线程与节点线程同步）
struct MiningCoordinator {
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> winner_found{false};
    int winner_id = -1;
    std::string winner_name;
    std::unique_ptr<Block> winner_block;
};

// User definition goes here
class User{
public:
    User() = default;
    // Create a new user with a given account
    explicit User(Account&& account);

    explicit User(const User&& other) noexcept;
    //get the account of the user
    Account& get_account();
    const Account& get_account() const;
    // 添加交易到交易池
    void add_transaction(const Transaction& tx);
    // 获取交易池中的交易数量
    size_t transaction_pool_size() const;
    // 取出交易池中的所有交易（移动语义）
    std::vector<Transaction> take_transaction_pool();
    // 获取区块链引用
    Blockchain& get_blockchain();
    // 获取区块链常引用
    const Blockchain& get_blockchain() const;
    // 获取节点名称
    std::string get_name() const;
    /// @brief 创建一笔交易（发送方为当前节点）
    Transaction create_transaction(const std::string& recipient_name, double amount, u_int64_t tx_id, const std::vector<u_int8_t>& private_key) const;
    /// @brief 从交易池打包并挖矿
    MiningResult mine_pending_block(u_int32_t difficulty);
    /// @brief 启动节点线程
    void start(int id, int difficulty, MiningCoordinator* coordinator);
    /// @brief 停止节点线程
    void stop();
    /// @brief 等待线程结束
    void join();
    /// @brief 发送消息到节点
    void send_message(const UserMessage& msg);
private:
    Account _account;
    std::vector<Transaction> _transactionPool;
    Blockchain _blockchain;
    // 线程相关成员
    int _id;
    int _difficulty;
    MiningCoordinator* _coordinator;
    std::thread _thread;
    std::queue<UserMessage> _message_queue;
    mutable std::mutex _queue_mutex;
    std::condition_variable _cv_message;
    std::atomic<bool> _running{false};
    u_int64_t _coinbase_tx_id_counter = 1000000;  // coinbase 交易 ID 基数

    // 内部事件处理
    void run();
    void handle_new_transaction(const Transaction& tx);
    void handle_mining();
    void handle_block_mined(const Block& block);
};
#endif // USER_HPP