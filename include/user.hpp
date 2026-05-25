#ifndef USER_HPP
#define USER_HPP
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "account.hpp"
#include "transaction.hpp"
#include "blockChain.hpp"
// User definition goes here
class User{
public:
    User() = default;
    // Create a new user with a given account
    explicit User(Account&& account);

    explicit User(const User&& other) noexcept;
    //get the account of the user
    const Account& get_account() const;
private:
    Account _account;
    std::vector<Transaction> _transactionPool;
    Blockchain _blockchain;
    
};
#endif // USER_HPP