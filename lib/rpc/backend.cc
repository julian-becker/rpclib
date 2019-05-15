#include <rpc/backend.h>
#include <string>
#include <rpc/server.h>
#include "rpc/detail/thread_group.h"
#include "rpc/detail/server_session.h"
#include "rpc/detail/dev_utils.h"
#include "rpc/detail/log.h"
#include "asio.hpp"

using RPCLIB_ASIO::ip::tcp;
using namespace RPCLIB_ASIO;
using namespace rpc;
using namespace rpc::detail;


struct msgpack_backend_impl : public backend::impl {
    msgpack_backend_impl(server<rpc::backend::msgpack> *parent, std::string const &address, uint16_t port)
        : parent_(parent),
          io_(),
          acceptor_(io_,
                    tcp::endpoint(ip::address::from_string(address), port)),
          socket_(io_),
          suppress_exceptions_(false) {}

    msgpack_backend_impl(server<rpc::backend::msgpack> *parent, uint16_t port)
        : parent_(parent),
          io_(),
          acceptor_(io_, tcp::endpoint(tcp::v4(), port)),
          socket_(io_),
          suppress_exceptions_(false) {}

    virtual void start_accept() override {
        acceptor_.async_accept(socket_, [this](std::error_code ec) {
            if (!ec) {
                LOG_INFO("Accepted connection.");
                auto s = std::make_shared<server_session>(
                    parent_, &io_, std::move(socket_), parent_->getDispatcher(),
                    suppress_exceptions_);
                s->start();
                sessions_.push_back(s);
            } else {
                LOG_ERROR("Error while accepting connection: {}", ec);
            }
            start_accept();
            // TODO: allow graceful exit [sztomi 2016-01-13]
        });
    }

    virtual void close_sessions() override { 
        for (auto &session : sessions_) {
            session->close();
        }
        sessions_.clear();
    }

    virtual void close_session(std::shared_ptr<rpc::detail::server_session> const &s) override {
        auto it = std::find(begin(sessions_), end(sessions_), s);
        if (it != end(sessions_)) {
            sessions_.erase(it);
        }
    }

    virtual void run() override {
        io_.run();
    }

    virtual void stop() override {
        io_.stop();
        loop_workers_.join_all();
    }

    virtual void suppress_exceptions(bool suppress) override {
        suppress_exceptions_.store(suppress);
    }

    virtual void async_run(std::size_t worker_threads) override {
        loop_workers_.create_threads(worker_threads, [this]() {
            name_thread("server");
            LOG_INFO("Starting");
            io_.run();
            LOG_INFO("Exiting");
        });
    }

    server<rpc::backend::msgpack> *parent_;
    io_service io_;
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    rpc::detail::thread_group loop_workers_;
    std::vector<std::shared_ptr<server_session>> sessions_;
    std::atomic_bool suppress_exceptions_;
    RPCLIB_CREATE_LOG_CHANNEL(server)
};