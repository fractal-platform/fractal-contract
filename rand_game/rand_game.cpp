#include "rand_game.hpp"
#include <vector>
#include <stdlib.h>

using namespace ftl;
using namespace std;

uint64_t rand_game::gen_rand(address player, uint64_t source1, uint64_t source2) {
    uint64_t result = source1 ^source2;
    if (result == 0xffffffffffffffff || result == 0xfffffffffffffffe || result == 0xfffffffffffffffd) {
        log("Wrong random,please bet again!");

        game _game = data.get(player);
        uint64_t amount1 = _game.amount1;
        uint64_t amount2 = _game.amount2;
        address owner = get_owner_address();

        transfer(player, amount1);
        transfer(owner, amount2);
        data.erase(player);
    }
    return result;
}

uint64_t rand_game::cal_rate(vector <uint8_t> opinion) {
    auto size = opinion.size();

    switch (size) {
        case 1:
            return 488;
            break;
        case 2:
            return 194;
            break;
        case 3:
            return 96;
            break;
        case 4:
            return 47;
            break;
        case 5:
            return 18;
            break;
        default:
            return 0;
    }
}

address rand_game::play_game(address player, uint64_t rand, vector <uint8_t> opinion) {
    uint8_t lucky = (uint8_t)(rand % 6 + 1);
    //log("RESURT", lucky);

    for (auto i : opinion) {
        if (i == lucky) {
            return player;
        }
    }
    return get_owner_address();
}

//<<<<<<<<<<<=======================================================================>>>>>>>>>>>>>

void rand_game::create(vector <uint8_t> opinion, checksum256 hash) {
    address player = get_from_address();

    if (data.has_key(player)) {
        check(data.get(player).opened == true, "You have a pending bet!");
        print("remove the play\n\n");
        data.erase(player);
    }

    uint64_t amount = get_transfer_amount();

    data.emplace([&](auto &s) {
        s.key.player = player;
        //memcpy(s.hash.hash,hash.hash, 32);
        s.hash = hash;
        s.opinion = opinion;
        s.amount1 = amount;
    });
}

void rand_game::retire() {
    address player = get_from_address();
    bool has = data.has_key(player);
    check(has == true, "not has running game.");

    uint64_t source = data.get(player).source;

    check(source == 0, "The game is begining!");

    transfer(player, data.get(player).amount1);

    data.erase(player);
}

void rand_game::react(address player, uint32_t s1, uint32_t s2) {
    uint64_t source = (((uint64_t) s1) << 32) ^(uint64_t) s2;

    address owner = get_owner_address();
    check(owner == get_from_address(), "only banker can react the bet");

    bool has = data.has_key(player);
    check(has == true, "not has running game.");

    game _game = data.get(player);

    check(_game.source == 0, "has already react.");
    uint64_t amount2 = (uint64_t)(_game.amount1 / 100 * cal_rate(_game.opinion));

    time_point current_time = current_time_point();
    uint64_t bank_amount = get_transfer_amount();

    if (bank_amount != amount2) {
        print("bank send wrong amount!");
        transfer(get_from_address(), bank_amount);
        return;
    }

    data.emplace([&](auto &s) {
        s.key.player = player;
        s.hash = _game.hash;
        s.amount1 = _game.amount1;
        s.amount2 = amount2;
        s.source = source;
        s.opinion = _game.opinion;
        s.time = current_time;
    });
    print("\nSuccessfully create a bet.\n");
}

void rand_game::open(uint32_t s1, uint32_t s2) {
    address player = get_from_address();
    bool has = data.has_key(player);
    check(has == true, "not has running game.");

    game _game = data.get(player);
    check(_game.source != 0, " Bank not accept the bet.");
    check(_game.opened == false, "Has opened");

    uint64_t source = (((uint64_t) s1) << 32) ^(uint64_t) s2;
    checksum256 hash = data.get(player).hash;
    assert_sha256((char *) &source, sizeof(source), &hash);

    uint64_t rand = gen_rand(player, source, _game.source);
    address winner = play_game(player, rand, _game.opinion);

    uint64_t total_amount = _game.amount1 + _game.amount2;
    transfer(winner, total_amount);

    data.emplace([&](auto &s) {
        s.key.player = player;
        s.hash = _game.hash;
        s.amount1 = _game.amount1;
        s.amount2 = _game.amount2;
        s.source = _game.source;
        s.opinion = _game.opinion;
        s.time = _game.time;
        s.winner = winner;
        s.opened = true;
    });

    print("\nWinner : ");
    print_address(winner);
    print("\n");
}

void rand_game::close(address player) {
    address owner = get_owner_address();

    check(owner == get_from_address(), "only banker can close the bet");
    game _game = data.get(player);
    microseconds delt_time = current_time_point() - _game.time;

    check(delt_time > microseconds(static_cast<int64_t>(300000)), "Not time out");

    uint64_t total_amount = _game.amount1 + _game.amount2;
    transfer(owner, total_amount);

    data.erase(player);
}

FTL_DISPATCH( rand_game, (create)(retire)
(react)(open)(close))

