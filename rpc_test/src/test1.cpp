#include <catch2/catch.hpp>
#include <rpc/rpc.h>
#include <rpc/server.h>

TEST_CASE("server can be instantiated", "[server_base_api]")
{
    using namespace rpc;
    REQUIRE_NOTHROW(server {});
}
