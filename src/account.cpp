#include "account.hpp"

Account::Account(std::string public_key, std::string private_key)
    : _public_key(std::move(public_key)), _private_key(std::move(private_key)), _balance(0.0) {}

std::string Account::get_public_key() const {
    return _public_key;
}

std::string Account::get_private_key() const {
    return _private_key;
}

double Account::get_balance() const {
    return _balance;
}