cmake_minimum_required(VERSION 3.28)

project(myblock_chain)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)


add_executable(block_chain 
    src/block.cpp
    src/blockChain.cpp
    src/transactionList.cpp
    src/util.cpp
    src/transaction.cpp
    src/main.cpp)

target_include_directories(block_chain PUBLIC ./include)

target_link_libraries(block_chain OpenSSL::Crypto)
