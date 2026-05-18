# myblock_chain

一个用 C++17 编写的简化版区块链项目，用于学习和理解区块链的基本概念。

## 功能

- 交易（Transaction）的创建与哈希计算（SHA-256）
- 交易列表（TransactionList）管理
- 区块（Block）与区块链（Blockchain）的框架设计
- 基础的数字签名占位逻辑（待完善）

> **当前状态**：项目处于早期开发阶段，区块和区块链的核心业务逻辑（如挖矿、链验证）尚未实现，详见 [待办事项](#待办事项)。

## 依赖

- **C++17** 或更高版本
- **CMake** ≥ 3.28
- **OpenSSL** 开发库（用于 SHA-256 哈希）

### 安装依赖（Ubuntu 示例）

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
