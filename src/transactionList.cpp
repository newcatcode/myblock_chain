#include "transactionList.hpp"
/// @brief Adds a transaction to the list
/// @param transaction 
void TransactionList::add_transaction(const Transaction& transaction) {
    _transactions.push_back(transaction);
}
/// @brief Gets all transactions
/// @return 
const std::vector<Transaction>& TransactionList::get_transactions() const {
    return _transactions;
}

/// @brief Converts the transaction list to a string representation
/// @return A string representation of the transaction list
std::string TransactionList::to_string() const {
    std::stringstream ss;
    for (const auto& transaction : _transactions) {
        ss << transaction.to_string() << "\n";
    }
    return ss.str();
}