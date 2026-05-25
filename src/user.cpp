#include "user.hpp"
User::User(Account&& account) : _account(std::move(account)) {};

User::User(const User&& other) noexcept : _account(std::move(other._account)), _transactionPool(std::move(other._transactionPool)), _blockchain(std::move(other._blockchain)) {}

const Account& User::get_account() const {
    return _account;
}