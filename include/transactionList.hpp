#ifndef TRANSACTIONLIST_HPP
#define TRANSACTIONLIST_HPP
#include <vector>
#include "transaction.hpp"
// TransactionList definition goes here
class TransactionList
{
public:
    TransactionList() = default;
    // Add a transaction to the list
    void add_transaction(const Transaction& transaction);
    // Get all transactions
    const std::vector<Transaction>& get_transactions() const;

    std::string to_string() const;
private:
    std::vector<Transaction> _transactions;
};
#endif // TRANSACTIONLIST_HPP