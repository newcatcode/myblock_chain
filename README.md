# myblock_chain

一个用 C++17 编写的简化版区块链项目，用于学习和理解区块链的基本概念。

## 功能

- **交易（Transaction）**：创建交易、SHA-256 哈希计算、签名与验证
- **区块（Block）**：区块结构、Merkle 根计算、工作量证明（PoW）挖矿
- **区块链（Blockchain）**：链式结构、创世区块、链完整性验证
- **账户系统（Account）**：基于 secp256k1 椭圆曲线的密钥对生成与地址派生
- **多节点模拟**：预生成 4 个独立节点，各自维护一条区块链
- **随机交易生成**：节点间随机产生交易（金额 1.0 ~ 50.0）
- **自动打包挖矿**：交易池满 8 笔时自动打包为区块
- **并行挖矿**：所有节点使用 pthread 并行挖矿，绑定 CPU 亲和性（`pthread_setaffinity_np`）

## 架构

```
                    ┌──────────────────────────┐
                    │      Transaction         │
                    │  - 交易ID / 发送方/接收方 │
                    │  - 金额 / 时间戳 / Nonce │
                    │  - SHA-256 哈希 / 签名   │
                    └────────────┬─────────────┘
                                 │ 多笔交易组成区块
                                 ▼
                    ┌──────────────────────────┐
                    │         Block            │
                    │  - 区块索引 / 时间戳     │
                    │  - 交易列表 / Merkle 根  │
                    │  - 前驱哈希 / 当前哈希   │
                    │  - PoW 挖矿 (Nonce)      │
                    └────────────┬─────────────┘
                                 │ 哈希链接成链
                                 ▼
                    ┌──────────────────────────┐
                    │       Blockchain         │
                    │  - 创世区块 + 区块列表   │
                    │  - add_block / 完整性验证 │
                    └────────────┬─────────────┘
                                 │ 每个节点独立持有
                     ┌───────────┴───────────┐
                     ▼                       ▼
              ┌──────────────────┐     ┌──────────────────┐
              │   User (×4)      │ ... │   User (×4)      │
              │ - Account        │     │ - Account        │
              │ - TxPool         │     │ - TxPool         │
              │ - Blockchain     │     │ - Blockchain     │
              │                  │     │                  │
              │ + create_tx()    │     │ + create_tx()    │
              │ + mine_block()   │     │ + mine_block()   │
              └──────────────────┘     └──────────────────┘
```

### 封装设计

所有节点操作封装在 `User` 类中：

| User 方法 | 职责 |
|-----------|------|
| `add_transaction(tx)` | 接收交易到交易池 |
| `create_transaction(recipient, amount, tx_id)` | 创建一笔新交易 |
| `mine_pending_block(difficulty)` | 从交易池打包区块 → PoW 挖矿 → 上链（一次性完成） |
| `get_blockchain()` | 获取该节点维护的区块链 |
| `get_name()` | 获取节点名称 |

## 核心流程

```
预生成 4 个节点 (Alice / Bob / Charlie / Dave)
    ↓
循环生成随机交易（随机发送方 / 接收方 / 金额）
    ↓
交易池满 8 笔 → 交易广播到所有节点的交易池
    ↓
启动 4 个 pthread 线程，绑定不同 CPU 核心
    ↓
每个线程调用 node->mine_pending_block(3)  ← 节点自行打包+挖矿+上链
    ↓
所有线程 join → 统计优胜节点（耗时最短）
    ↓
清空交易池 → 进入下一轮
    ↓
3 轮后打印所有节点的最终区块链状态 + 链完整性验证
```

## 代码结构

```
myblock_chain/
├── CMakeLists.txt              # CMake 构建配置 (C++17, OpenSSL)
├── README.md                   # 本文件
├── include/                    # 头文件
│   ├── account.hpp             # 账户：名称、余额、secp256k1 密钥对
│   ├── block.hpp               # 区块：索引、交易、Merkle、PoW
│   ├── blockChain.hpp          # 区块链：创世、追加、验证
│   ├── transaction.hpp         # 交易：ID、发送/接收方、金额、哈希、签名
│   ├── user.hpp                # 节点 + MiningResult 结构体
│   └── util.hpp                # 工具：密钥生成、地址派生、时间转换
└── src/                        # 实现文件
    ├── account.cpp
    ├── block.cpp
    ├── blockChain.cpp
    ├── main.cpp                # 入口：编排 4 节点创建、随机交易、并行挖矿
    ├── transaction.cpp
    ├── user.cpp                # 节点操作实现
    └── util.cpp
```

### 主要类说明

| 类 | 职责 | 关键方法 |
|----|------|----------|
| `Transaction` | 交易数据与哈希 | `compute_hash()`, `sign_transaction()`, `verify_signature()` |
| `Block` | 区块结构与挖矿 | `mine_block(difficulty)`, `calculate_merkle_root()`, `get_nonce()` |
| `Blockchain` | 链式存储与验证 | `add_block()`, `is_chain_valid()`, `to_string()` |
| `Account` | 用户账户与密钥 | `get_name()`, `get_public_key()`, `get_private_key()`, `get_balance()` |
| `User` | 节点操作封装 | `create_transaction()`, `mine_pending_block()`, `add_transaction()`, `get_blockchain()` |
| `MiningResult` | 挖矿结果（user.hpp） | `node_name`, `nonce`, `duration_ms`, `success` |

## 依赖

- **C++17** 或更高版本
- **CMake** ≥ 3.28
- **OpenSSL** 开发库（SHA-256 哈希 + secp256k1 椭圆曲线）
- **pthread**（CPU 亲和性绑定，实现并行挖矿）

### 安装依赖（Ubuntu）

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
```

## 构建与运行

```bash
cd myblock_chain
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/block_chain
```

### 预期输出

```
User1 (Alice) Address: a1b2c3...
User2 (Bob) Address: d4e5f6...
User3 (Charlie) Address: g7h8i9...
User4 (Dave) Address: j0k1l2...

========== 开始区块链模拟 ==========
节点数量: 4, 难度: 3, 每块交易数: 8, 轮数: 3

========== 第 1 轮挖矿 ==========
  生成交易 #0: Alice -> Bob 23.45 单位
  生成交易 #1: Charlie -> Dave 12.78 单位
  ...

交易池已满 (8 笔)，所有节点开始并行挖矿...

挖矿结果:
  [Alice] Nonce: 12345, 耗时: 15.23 ms ✓
  [Bob] Nonce: 67890, 耗时: 8.47 ms ✓
  [Charlie] Nonce: 34567, 耗时: 12.01 ms ✓
  [Dave] Nonce: 98765, 耗时: 20.56 ms ✓
>>> 优胜节点: Bob (耗时 8.47 ms)
区块 #1 已加入所有节点的区块链

...

========== 最终区块链状态 ==========

----- Alice 的区块链 (4 个区块) -----
...
Hash: 000abc...

========== 链完整性验证 ==========
[Alice] 区块链 ✓ 有效
[Bob] 区块链 ✓ 有效
[Charlie] 区块链 ✓ 有效
[Dave] 区块链 ✓ 有效
```

## 设计要点

- **工作量证明**：SHA-256 哈希前导零匹配，difficulty=3（约 4096 次尝试）
- **Merkle 根**：二叉 Merkle 树，交易数奇数时复制最后一个节点
- **并行挖矿**：每个节点独立创建 Block 副本 → `pthread` 线程 → `pthread_setaffinity_np` 绑定 CPU → 无数据竞争
- **密钥体系**：secp256k1 椭圆曲线生成 32 字节私钥 → 65 字节非压缩公钥 → SHA-256 前 20 字节作为地址
