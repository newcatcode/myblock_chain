#include "user.hpp"
#include <chrono>
User::User(Account&& account) : _account(std::move(account)) {};

User::User(const User&& other) noexcept : _account(std::move(other._account)), _transactionPool(std::move(other._transactionPool)), _blockchain(std::move(other._blockchain)) {}

Account& User::get_account() {
    return _account;
}

const Account& User::get_account() const {
    return _account;
}

void User::add_transaction(const Transaction& tx) {
    _transactionPool.push_back(tx);
}

size_t User::transaction_pool_size() const {
    return _transactionPool.size();
}

std::vector<Transaction> User::take_transaction_pool() {
    std::vector<Transaction> pool;
    std::swap(pool, _transactionPool);
    return pool;
}

Blockchain& User::get_blockchain() {
    return _blockchain;
}

const Blockchain& User::get_blockchain() const {
    return _blockchain;
}

std::string User::get_name() const {
    return _account.get_name();
}

Transaction User::create_transaction(const std::string& recipient_name, double amount, u_int64_t tx_id, const std::vector<u_int8_t>& private_key) const {
    Transaction tx(tx_id, get_name(), recipient_name, amount);
    tx.sign_transaction(private_key);
    return tx;
}

// 创建 coinbase 交易（奖励矿工）
static Transaction create_coinbase(u_int64_t tx_id, const std::string& miner_name, double reward) {
    Transaction tx(tx_id, "", miner_name, reward);
    // coinbase 不签名，发送方为空
    return tx;
}

MiningResult User::mine_pending_block(u_int32_t difficulty) {
    MiningResult result;
    result.node_name = get_name();

    auto start = std::chrono::high_resolution_clock::now();

    // 使用交易池中的交易打包区块
    std::string prev_hash = _blockchain.get_latest_block_hash();
    u_int64_t block_index = _blockchain.get_chain_length();
    std::vector<Transaction> txs = std::move(_transactionPool);
    _transactionPool.clear();

    Block block(block_index, std::move(txs), prev_hash);
    result.success = block.mine_block(difficulty);
    result.nonce = block.get_nonce();

    auto end = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    if (result.success) {
        _blockchain.add_block(block);
    }

    return result;
}

void User::start(int id, int difficulty, MiningCoordinator* coordinator) {
    _id = id;
    _difficulty = difficulty;
    _coordinator = coordinator;
    _running = true;
    _thread = std::thread(&User::run, this);
}

void User::stop() {
    send_message({UserMsgType::STOP, {}, {}});
}

void User::join() {
    if (_thread.joinable()) {
        _thread.join();
    }
}

void User::send_message(const UserMessage& msg) {
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _message_queue.push(msg);
    _cv_message.notify_one();
}

void User::run() {
    while (_running) {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _cv_message.wait(lock, [this]() { return !_message_queue.empty() || !_running; });

        while (!_message_queue.empty()) {
            UserMessage msg = std::move(_message_queue.front());
            _message_queue.pop();
            lock.unlock();

            switch (msg.type) {
                case UserMsgType::ADD_TRANSACTION:
                    handle_new_transaction(msg.tx);
                    break;
                case UserMsgType::START_MINING:
                    handle_mining();
                    break;
                case UserMsgType::BLOCK_MINED:
                    handle_block_mined(msg.block);
                    break;
                case UserMsgType::STOP:
                    _running = false;
                    return;
            }

            lock.lock();
        }
    }
}

void User::handle_new_transaction(const Transaction& tx) {
    _transactionPool.push_back(tx);
    std::cout << "  [" << get_name() << "] 收到交易 #" << tx.get_id() << std::endl;
}

void User::handle_mining() {
    auto start = std::chrono::high_resolution_clock::now();

    std::string prev_hash = _blockchain.get_latest_block_hash();
    u_int64_t block_index = _blockchain.get_chain_length();
    std::vector<Transaction> txs = std::move(_transactionPool);
    _transactionPool.clear();

    // 第一笔交易为 coinbase（奖励矿工）
    double reward = (block_index == 0) ? 100.0 : 50.0;
    Transaction coinbase = create_coinbase(_coinbase_tx_id_counter++, get_name(), reward);
    txs.insert(txs.begin(), std::move(coinbase));

    Block block(block_index, std::move(txs), prev_hash);
    block.mine_block(_difficulty);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();

    // 尝试成为优胜者（原子CAS，仅最先完成的节点胜出）
    bool expected = false;
    if (_coordinator->winner_found.compare_exchange_strong(expected, true)) {
        {
            std::lock_guard<std::mutex> lock(_coordinator->mtx);
            _coordinator->winner_id = _id;
            _coordinator->winner_name = get_name();
            _coordinator->winner_block = std::make_unique<Block>(block);
        }
        _coordinator->cv.notify_one();
        std::cout << "  [" << get_name() << "] ★ 挖矿成功! Nonce: " << block.get_nonce()
                  << ", 耗时: " << duration << " ms" << std::endl;
    } else {
        std::cout << "  [" << get_name() << "] 挖矿完成，但 " << _coordinator->winner_name
                  << " 已抢先" << std::endl;
    }
}

void User::handle_block_mined(const Block& block) {
    // 验证前驱哈希
    if (block.get_previous_hash() != _blockchain.get_latest_block_hash()) {
        std::cout << "  [" << get_name() << "] 区块前驱哈希不匹配，丢弃" << std::endl;
        return;
    }
    // 验证交易
    if (!block.verify_transactions()) {
        std::cout << "  [" << get_name() << "] 区块交易验证失败，丢弃" << std::endl;
        return;
    }
    // 根据区块中的交易更新本节点余额
    for (const auto& tx : block.get_transactions()) {
        if (tx.get_sender().empty()) {
            // Coinbase 交易（金额由区块创建者设定）
            if (tx.get_recipient() == get_name()) {
                _account.add_balance(tx.get_amount());
                std::cout << "  [" << get_name() << "] 获得 coinbase 奖励 " << tx.get_amount() << std::endl;
            }
        } else {
            if (tx.get_sender() == get_name()) {
                _account.add_balance(-tx.get_amount());  // 扣除发送金额
            }
            if (tx.get_recipient() == get_name()) {
                _account.add_balance(tx.get_amount());   // 增加接收金额
            }
        }
    }
    // 添加到链
    _blockchain.add_block(block);
    std::cout << "  [" << get_name() << "] 已添加区块 #"
              << (_blockchain.get_chain_length() - 1)
              << " (余额: " << _account.get_balance() << ")" << std::endl;
}