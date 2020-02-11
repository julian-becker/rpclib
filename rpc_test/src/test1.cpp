#include <catch2/catch.hpp>
#include <rpc/rpc.h>

TEST_CASE("rpc_api_foobar returns the ultimate result", "[rpc_api_foobar]")
{
    REQUIRE(42 == rpc_api());
}
