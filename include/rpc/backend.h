#pragma once

#ifndef BACKEND_H_98769876
#define BACKEND_H_98769876
#include <cstdint>
#include <memory>
#include <string>

namespace rpc {

namespace detail {
class server_session;
}

namespace backend{

struct impl {
    virtual ~impl() noexcept {}

    virtual void start_accept() = 0;
    virtual void close_session(std::shared_ptr<detail::server_session> const &s) = 0;
    virtual void close_sessions() = 0;
    virtual void stop() = 0;
    virtual void run() = 0;
    virtual void suppress_exceptions(bool suppress) = 0;
    virtual void async_run(std::size_t worker_threads) = 0;
};

class msgpack {
public:
    //! \brief Constructs a server that listens on the localhost on the
    //! specified port.
    //!
    //! \param port The port number to listen on.
    static auto create(std::uint16_t port) -> std::shared_ptr<impl>;

    //! \brief Constructs a server that listens on the specified address on
    //! the specified port.
    //!
    //! \param address The address to bind to. This only works if oee of your
    //! network adapaters control the given address.
    //! \param port The port number to listen on.
    static auto create(std::string const &address, std::uint16_t port) -> std::shared_ptr<impl>;
};

}

}

#endif