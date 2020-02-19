#pragma once
#include "co.hpp"
#include "net.hpp"
#include "socket_addr.hpp"
#include "socket_buffer.hpp"
#include <functional>
#include <memory>

namespace net
{
class event_context_t;
class rudp_impl_t;

class rudp_t
{
  public:
    using data_handler_t = std::function<void(rudp_t &, socket_buffer_t buffer)>;

    /// return false will discard current packet
    using unknown_handler_t = std::function<bool(socket_addr_t address)>;
    using timeout_handler_t = std::function<void(socket_addr_t)>;

  private:
    // impl idiom
    /// 隔离第三方库
    rudp_impl_t *impl;

  public:
    rudp_t();
    ~rudp_t();

    rudp_t(const rudp_t &) = delete;
    rudp_t &operator=(const rudp_t &) = delete;

    /// bind a local address
    void bind(event_context_t &context, socket_addr_t local_addr, bool reuse_addr = false);

    /// bind random port
    void bind(event_context_t &context);

    /// addr remote address
    void add_connection(socket_addr_t addr, int channel, microsecond_t inactive_timeout);

    void config(socket_addr_t addr, int channel, bool fast_mode, int level);

    void set_wndsize(socket_addr_t addr, int channel, int send, int recv);

    /// clear remote address restrictions
    void remove_connection(socket_addr_t addr, int channel);

    bool removeable(socket_addr_t addr, int channel);

    rudp_t &on_unknown_packet(unknown_handler_t handler);

    rudp_t &on_connection_timeout(timeout_handler_t handler);

    /// call 'bind' before call this function
    void run(std::function<void()> func);

    co::async_result_t<io_result> awrite(co::paramter_t &param, socket_buffer_t &buffer, socket_addr_t address,
                                         int channel);
    co::async_result_t<io_result> aread(co::paramter_t &param, socket_buffer_t &buffer, socket_addr_t &address,
                                        int &channel);

    socket_t *get_socket() const;

    void close_all_remote();

    int get_mtu() const
    {
        return 1472 - 24; // kcp header 24
    }

    void close();
};

// wrapper functions
co::async_result_t<io_result> rudp_awrite(co::paramter_t &param, rudp_t *rudp, socket_buffer_t &buffer,
                                          socket_addr_t address, int channel);
co::async_result_t<io_result> rudp_aread(co::paramter_t &param, rudp_t *rudp, socket_buffer_t &buffer,
                                         socket_addr_t &address, int &channel);

} // namespace net
