# ifndef ACCOUNT_HPP
# define ACCOUNT_HPP
#include <string>
// Account definition goes here
class Account{
public:
    Account() = default;
    // Create a new account with a given public key and private key
    Account(std::string public_key, std::string private_key);
    // Get the public key of the account
    std::string get_public_key() const;
    // Get the private key of the account
    std::string get_private_key() const;
private:
    // user's name
    std::string _name;
    // user's money balance
    double _balance;
    // user's public key
    std::string _public_key;
    // user's private key
    std::string _private_key;
};
# endif // ACCOUNT_HPP