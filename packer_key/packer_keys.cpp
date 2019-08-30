#include <ftllib/contract.hpp>
#include <ftllib/dispatcher.hpp>
#include <ftllib/map.hpp>
#include <ftllib/crypto.hpp>
#include <ftllib/address.hpp>


using namespace ftl;
using namespace std;

class [[ftl::contract("packer_keys")]]  packer_keys : public contract {

public:
    using contract::contract;

    packer_keys(datastream<const char *> ds) : contract(ds) {}

    [[ftl::action]]
    void setkey(uint32_t index, string ip, address addr, vector <uint8_t> pub_key) {
        print("set packer pub key\n");
        address owner = get_owner_address();
        check(owner == get_from_address(), "only owner can set the packer key.");

        uint8_t size_key = 0;
        check(pub_key.size() == 65, "the length of public key is not equal to 65.");
        packer_pub_keys keys;
        packer_size size;
        auto has = keys.has_key(index);
        if (!has) {
            auto has_size = size.has_key(size_key);
            if (!has_size) {
                size.emplace([&](auto &row) {
                    row.key.packers = size_key;
                    row.size = 1;
                });
            } else {
                auto souce_size = size.get(size_key).size;
                size.emplace([&](auto &row) {
                    row.key.packers = size_key;
                    row.size = souce_size + 1;
                });
            }
        }

        keys.emplace([&](auto &row) {
            row.key.index = index;
            row.packerInfo.ip = ip;
            row.packerInfo.addr = addr;
            row.packerInfo.pubkey = pub_key;
        });
    }

    [[ftl::action]]
    void deletekey(uint32_t index) {
        print("delete packer pub keys\n");
        address owner = get_owner_address();
        check(owner == get_from_address(), "only owner can set the delete key.");
        uint8_t size_key = 0;
        packer_pub_keys keys;
        if (!keys.has_key(index)) {
            return;
        }

        keys.erase(index);
        packer_size size;

        auto souce_size = size.get(size_key).size;

        check(souce_size > 0, "The size of pakcers is less than 0");

        size.emplace([&](auto &row) {
            row.key.packers = size_key;
            row.size = souce_size - 1;
        });
    }

private:
    struct packer {
        address addr;
        vector <uint8_t> pubkey;
        string ip;
    };

    struct [[ftl::table]] packerkey {
        struct _key {
            uint32_t index; //这个东西类型必须匹配/
        } key;
        packer packerInfo;
    };

    typedef ftl::table<"packerkey"_n, packerkey> packer_pub_keys;

    struct [[ftl::table]] packersize {
        struct _key {
            uint8_t packers = 0;
        } key;
        uint32_t size;
    };

    typedef ftl::table<"packersize"_n, packersize> packer_size;
};

FTL_DISPATCH( packer_keys, (setkey)(deletekey)
)
