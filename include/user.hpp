#ifndef USER_HPP
#define USER_HPP
#include <string>
#include <vector>
#include "account.hpp"
#include "transaction.hpp"
// User definition goes here
class User{
public:

private:
    Account _account;
    std::vector<Transaction> transactionPool;
    
};
#endif // USER_HPP