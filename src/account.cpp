#include "account.hpp"

Account::Account(std::string name,std::vector<u_int8_t> private_key,std::vector<u_int8_t> public_key)
    : _name(std::move(name)), _private_key(std::move(private_key)),_public_key(std::move(public_key)), _balance(0.0) {}

Account::Account(const Account&& other) noexcept    : _name(std::move(other._name)),
      _balance(other._balance),
      _public_key(std::move(other._public_key)),
      _private_key(std::move(other._private_key)) {}
      

std::vector<u_int8_t> Account::get_public_key() const {
    return _public_key;
}

std::vector<u_int8_t> Account::get_private_key() const {
    return _private_key;
}

double Account::get_balance() const {
    return _balance;
}

void Account::set_balance(double balance) {
    _balance = balance;
}
