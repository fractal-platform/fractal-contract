#pragma once

#include <ftllib/print.hpp>
#include <ftllib/map.hpp>
#include <ftllib/contract.hpp>
#include <ftllib/address.hpp>
#include <ftllib/asset.hpp>
#include <ftllib/dispatcher.hpp>
#include <ftllib/symbol.hpp>


using namespace ftl;
using namespace std;

class [[ftl::contract("token")]] token : public contract {
public:
    using contract::contract;

    [[ftl::action]]
    void create(asset maximum_supply);

    [[ftl::action]]
    void issue(address to, asset quantity);

    [[ftl::action]]
    void retire(address from, asset quantity);

    [[ftl::action]]
    void transfer(address to, asset quantity);

private:

    struct [[ftl::table]] account {
        struct _key {
            address owner;
            uint64_t type;
        } key;

        uint64_t balance;

    };

    struct [[ftl::table]] stat {
        struct _key {
            uint64_t type;
        } key;

        uint64_t supply;
        uint64_t max_supply;
        address issuer;
    };

    typedef table<"account"_n, account> accounts;
    typedef table<"stat"_n, stat> stats;

    void sub_balance(address, asset value);

    void add_balance(address, asset value);

};

