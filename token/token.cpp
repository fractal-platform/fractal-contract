#include "token.hpp"
#include <vector>

using namespace ftl;
using namespace std;

void token::create(asset maximum_supply) {
    //TODO:require_auth
    address issuer = get_from_address();
    auto sym = maximum_supply.symbol;

    check(sym.is_valid(), "invalid symbol name");
    check(maximum_supply.is_valid(), "invalid supply");
    check(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable;
    bool existing = statstable.has_key(maximum_supply.symbol.code().raw());
    check(existing == false, "token with symbol already exists");

    statstable.emplace([&](auto &s) {
        s.key.type = maximum_supply.symbol.code().raw();
        s.max_supply = maximum_supply.amount;
        s.issuer = issuer;
    });
    print("\nCreate token");
}

void token::issue(address to, asset quantity) {
    auto sym = quantity.symbol;
    check(sym.is_valid(), "invalid symbol name");

    stats statstable;
    auto existing = statstable.has_key(sym.code().raw());
    check(existing == true, "token with symbol does not exist, create token before issue");

    stat st = statstable.get(sym.code().raw());
    //TODO:require_auth
    address issuer = get_from_address();

    check(issuer == st.issuer, "Wrong issuer");
    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must issue positive quantity");

    check(quantity.amount <= st.max_supply - st.supply, "quantity exceeds available supply");

    statstable.emplace([&](auto &s) {
        s.key.type = sym.code().raw();
        s.supply += quantity.amount;
        s.max_supply = st.max_supply;
        s.issuer = st.issuer;
    });

    add_balance(to, quantity);
    print("\nissue token");
}

void token::retire(address from, asset quantity) {
    auto sym = quantity.symbol;
    check(sym.is_valid(), "invalid symbol name");

    stats statstable;
    auto existing = statstable.has_key(sym.code().raw());
    check(existing == true, "token with symbol does not exist, create token before issue");

    stat st = statstable.get(sym.code().raw());
    //TODO:require_auth
    address issuer = get_from_address();

    check(issuer == st.issuer, "Wrong issuer");
    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must retire positive quantity");

    check(quantity.amount <= st.max_supply, "quantity exceeds available supply");
    print("Retire token");
    sub_balance(from, quantity);
}

void token::transfer(address to, asset quantity) {

    address from = get_from_address();
    check(from != to, "cannot transfer to self");

    auto sym = quantity.symbol;
    stats statstable;
    const auto &st = statstable.get(sym.code().raw());

    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must transfer positive quantity");

    print("Transfer token");
    sub_balance(from, quantity);
    add_balance(to, quantity);
}

void token::add_balance(address owner, asset value) {
    check(value.amount > 0, "add negative assert");

    accounts account;
    bool has = account.has_key(owner, value.symbol.code().raw());
    if (!has) {
        account.emplace([&](auto &s) {
            s.key.owner = owner;
            s.key.type = value.symbol.code().raw();
            s.balance = value.amount;
        });
    } else {
        account.emplace([&](auto &s) {
            s.key.owner = owner;
            s.key.type = value.symbol.code().raw();
            s.balance += value.amount;
        });
    }
}

void token::sub_balance(address owner, asset value) {
    check(value.amount > 0, "sub negative assert");
    accounts account;
    bool has = account.has_key(owner, value.symbol.code().raw());
    check(has == true, "This is an empty account!");
    check(account.get(owner, value.symbol.code().raw()).balance >= value.amount,
          "account don not has enough tokens");

    account.emplace([&](auto &s) {
        s.key.owner = owner;
        s.key.type = value.symbol.code().raw();
        s.balance -= value.amount;
    });
}

FTL_DISPATCH(token, (create)(issue)
(transfer)(retire))


