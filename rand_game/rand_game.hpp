#pragma once

#include <ftllib/print.hpp>
#include <ftllib/address.hpp>
#include <ftllib/map.hpp>
#include <ftllib/contract.hpp>
#include <ftllib/log.hpp>
#include <ftllib/dispatcher.hpp>
#include <ftllib/system.hpp>
#include <ftllib/time.hpp>
#include <ftllib/action.hpp>

using namespace std;
using namespace ftl;

class [[ftl::contract("rand_game")]] rand_game : public ftl::contract {

private:

    struct [[ftl::table]] game {
        struct _key {
            address player;
        } key;

        vector <uint8_t> opinion;
        uint64_t amount1;
        uint64_t amount2;
        checksum256 hash;
        uint64_t source = 0;
        address winner;
        bool opened = false;
        time_point time = time_point(microseconds(static_cast<int64_t>(0xfffffffffffffff)));
    };

    typedef table<"game"_n, game> game_data;

    uint64_t cal_rate(vector <uint8_t> opinion);

    address play_game(address player, uint64_t rand, vector <uint8_t> opinion);

    uint64_t gen_rand(address player, uint64_t source1, uint64_t source2);

    game_data data;

public:

    rand_game(datastream<const char *> ds) : contract(ds) {};

    [[ftl::action]]
    void create(vector <uint8_t> opinion, checksum256 hash);

    [[ftl::action]]
    void retire();

    [[ftl::action]]
    void react(address player, uint32_t s1, uint32_t s2);

    [[ftl::action]]
    void open(uint32_t s1, uint32_t s2);

    [[ftl::action]]
    void close(address player);
};
