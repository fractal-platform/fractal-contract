#include <ftllib/contract.hpp>
#include <ftllib/dispatcher.hpp>
#include <ftllib/map.hpp>


using namespace ftl;
using namespace std;

class [[ftl::contract("roster")]] roster : public contract {
public:

    roster(datastream<const char *> ds) : contract(ds) {}

    [[ftl::action]]
    void upsert(string name, string sex, string age, string birthday, string situation) {

        member_list mems;
        mems.emplace([&](auto &r) {
            r.key.name = name;
            r.sex = sex;
            r.age = age;
            r.birthday = birthday;
            r.situation = situation;
        });
    }

    [[ftl::action]]
    void erase(string name) {
        member_list mems;
        bool exist = mems.has_key(name);
        check(exist == false, "Record does not exist");
        mems.erase(name);
    }

private:
    struct [[ftl::table]] member {
        struct _key {
            string name;
        } key;

        string sex;
        string age;
        string birthday;
        string situation;

    };
    typedef table<"members"_n, member> member_list;
};

FTL_DISPATCH(roster, (upsert)(erase)
)

