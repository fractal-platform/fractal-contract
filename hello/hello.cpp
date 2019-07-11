#include <ftllib/contract.hpp>
#include <ftllib/dispatcher.hpp>

using namespace ftl;

class [[ftl::contract("hello")]] hello : public contract {
public:
    hello(datastream<const char *> ds) : contract(ds) {}

    [[ftl::action]]
    void hi(std::string user) {
        print("Hello, ", user);
    }
};

FTL_DISPATCH(hello, (hi)
)
