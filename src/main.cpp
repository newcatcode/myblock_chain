#include <iostream>
#include <vector>
#include <thread>
#include <unordered_map>
#include "util.hpp"
#include "account.hpp"
#include "user.hpp"
#include "blockChain.hpp"
#include "transaction.hpp"
int main() {
    // address pool
    std::vector<std::string> addresses;
    // user pool
    std::unordered_map<std::string, User*> users;
    // init a user
    Account account1("Alice",util::generate_private_key(),util::generate_public_key(util::generate_private_key()));
    User* user1 = new User(std::move(account1));
    std::string user1_address = util::address_from_public_key(user1->get_account().get_public_key());
    users[user1_address] = user1;
    addresses.push_back(user1_address);
    std::cout << "User1 Address: " << user1_address << std::endl;
}