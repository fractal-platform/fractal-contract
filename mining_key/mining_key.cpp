#include <ftllib/contract.hpp>
#include <ftllib/dispatcher.hpp>
#include <ftllib/log.hpp>
#include <ftllib/map.hpp>
#include <ftllib/address.hpp>

using namespace ftl;

class [[ftl::contract("mining_key")]] mining_key : public ftl::contract {

public:
    mining_key(datastream<const char *> ds) : contract(ds) {}

    [[ftl::action]]
    void setkey(std::vector <uint8_t> bls_key) {
        address addr = get_from_address();
        bls_keys keys;
        keys.emplace([&](auto &row) {
            row.key.addr = addr;
            row.minerkey = bls_key;
        });
    }

private:
    struct [[ftl::table]] minerkey {
        struct _key {
            address addr;
        } key;

        std::vector <uint8_t> minerkey;
    };

    typedef table<"minerkey"_n, minerkey> bls_keys;

};

FTL_DISPATCH( mining_key, (setkey)
)