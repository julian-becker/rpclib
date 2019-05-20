#include <chrono>
#include <memory>

#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "rpc/this_session.h"
#include "rpc/rpc_error.h"
#include "rpc/detail/make_unique.h"
#include "testutils.h"

using namespace rpc::testutils;

class server_session_test : public testing::Test {
public:
    server_session_test() :
            s("127.0.0.1", test_port),
            c("127.0.0.1", test_port) {
        s.bind("consume_big_param", [](std::string const& str){ (void)str; });
        s.bind("func", [](){ return 0; });
        s.bind("get_sid", [](){ return rpc::this_session().id(); });
        s.async_run();
    }

protected:
    static const int test_port = rpc::constants::DEFAULT_PORT;
    rpc::server<rpc::backend::msgpack> s;
    rpc::client<rpc::backend::msgpack_client> c;
};

TEST_F(server_session_test, consume_big_param) {
    std::size_t blob_size = 2 << 10 << 10;
    for (int i = 0; i < 4; ++i) {
        c.call("consume_big_param", get_blob(blob_size));
        blob_size *= 2;
    }
    // no crash is enough
}

TEST_F(server_session_test, connection_closed_properly) {
#ifdef RPCLIB_WIN32
	const unsigned max_tries = 10;
#else
	const unsigned max_tries = 1000;
#endif
    for (unsigned counter = 0; counter < max_tries; ++counter) {
        rpc::client<rpc::backend::msgpack_client> client("localhost", rpc::constants::DEFAULT_PORT);
        auto response = client.call("func");
    }
    // no crash is enough
}

TEST_F(server_session_test, session_id_unique) {
    rpc::client<rpc::backend::msgpack_client> c2("localhost", rpc::constants::DEFAULT_PORT);
    auto sid1 = c.call("get_sid").as<rpc::session_id_t>();
    auto sid2 = c2.call("get_sid").as<rpc::session_id_t>();
    EXPECT_NE(sid1, sid2);
}

TEST(server_session_test_bug153, bug_153_crash_on_client_timeout) {
    rpc::server<rpc::backend::msgpack> s("127.0.0.1", rpc::constants::DEFAULT_PORT);
    s.bind("bug_153", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 0;
    });
    s.async_run();

    auto client = std::unique_ptr<rpc::client<rpc::backend::msgpack_client>>(new rpc::client<rpc::backend::msgpack_client>("localhost", rpc::constants::DEFAULT_PORT));
    client->set_timeout(5);
  
    try {
        client->call("bug_153");
    } catch(rpc::timeout& ) {
        client.reset();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // no crash is enough
}
