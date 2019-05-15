#include "rpc/server.h"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <stdint.h>
#include <thread>

#include "asio.hpp"
#include "format.h"
#include "rpc/backend.h"
#include "rpc/detail/dev_utils.h"
#include "rpc/detail/log.h"
#include "rpc/detail/server_session.h"
#include "rpc/detail/thread_group.h"

using namespace rpc::detail;
using RPCLIB_ASIO::ip::tcp;
using namespace RPCLIB_ASIO;

namespace rpc {


RPCLIB_CREATE_LOG_CHANNEL(server)

template <>
server<rpc::backend::msgpack>::server(uint16_t port)    
    : pimpl(backend::msgpack::create(disp_, port))
    , disp_(std::make_shared<dispatcher<rpc::backend::msgpack>>())
{
    LOG_INFO("Created server on localhost:{}", port);
    pimpl->start_accept();
}

template <>
server<rpc::backend::msgpack>::server(server&& other) noexcept {
    *this = std::move(other);
}

template <>
server<rpc::backend::msgpack>::server(std::string const &address, uint16_t port)
    : pimpl(backend::msgpack::create(disp_, address, port))
    , disp_(std::make_shared<dispatcher<rpc::backend::msgpack>>()) {
    LOG_INFO("Created server on address {}:{}", address, port);
    pimpl->start_accept();
}

template <>
server<rpc::backend::msgpack>::~server() {
    if (pimpl) {
        pimpl->stop();
    }
}

template <>
server<rpc::backend::msgpack>& server<rpc::backend::msgpack>::operator=(server &&other) {
    pimpl = std::move(other.pimpl);
    other.pimpl = nullptr;
    disp_ = std::move(other.disp_);
    other.disp_ = nullptr;
    return *this;
}

template <>
void server<rpc::backend::msgpack>::suppress_exceptions(bool suppress) {
    pimpl->suppress_exceptions(suppress);
}

template <>
void server<rpc::backend::msgpack>::run() { pimpl->run(); }

template <>
void server<rpc::backend::msgpack>::async_run(std::size_t worker_threads) {
    pimpl->async_run(std::move(worker_threads));
}

template <>
void server<rpc::backend::msgpack>::stop() { pimpl->stop(); }

template <>
void server<rpc::backend::msgpack>::close_sessions() { pimpl->close_sessions(); }

template <>
void server<rpc::backend::msgpack>::close_session(std::shared_ptr<detail::server_session> const &s) {
  pimpl->close_session(s);
}

} /* rpc */
