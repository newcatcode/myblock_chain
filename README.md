# myblock_chain

一个用 C++17 编写的简化版区块链项目，用于学习和理解区块链的基本概念。

## 功能

- 交易（Transaction）的创建与哈希计算（SHA-256）
- 区块（Block）与区块链（Blockchain）的框架设计
- 账户系统（Account）和模拟分布式节点（user）
- main实现模式，待定

> **当前状态**：基础框架建立，节点并行未设定

## 依赖

- **C++17** 或更高版本
- **CMake** ≥ 3.28
- **OpenSSL** 开发库（用于 SHA-256 哈希）
- **pthread** 用于CPU核心绑定实现并行

### 安装依赖（Ubuntu 示例）

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
