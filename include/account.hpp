# ifndef ACCOUNT_HPP
# define ACCOUNT_HPP
#include <string>
#include <vector>
#include "util.hpp"
// Account definition goes here
class Account{
public:
    Account() = default;
    // Create a new account with a given public key and private key
    Account(std::string name,std::vector<u_int8_t> private_key,std::vector<u_int8_t> public_key);
    // Move constructor for Account
    Account(const Account&& other) noexcept;
    // Get the public key of the account
    std::vector<u_int8_t> get_public_key() const;
    // Get the private key of the account
    std::vector<u_int8_t> get_private_key() const;
    // Get the balance of the account
    double get_balance() const;
    // Set the balance of the account
    void set_balance(double balance);

private:
    // user's name
    std::string _name;
    // user's money balance
    double _balance;
    // user's private key
    std::vector<u_int8_t> _private_key;
    // user's public key
    std::vector<u_int8_t> _public_key;
};
# endif // ACCOUNT_HPP