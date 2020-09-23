/**
 *
 * @file ZmqSender.cpp ZmqSender messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmSender.hpp"
#include "ipm/ZmqContext.hpp"

#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqSender : public ipmSender
{
public:
  ZmqSender()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::push)
  {}
  bool can_send() const noexcept override;

protected:
  void send_(const void* message, int N, const duration_type& timeout) override;

private:
  zmq::socket_t socket_;
  bool socket_connected_;
};

bool
ZmqSender::can_send() const noexcept
{
  return socket_connected_;
}

void
ZmqSender::send_(const void* message, int N, const duration_type& )
{
  zmq::message_t msg(message, N);
  socket_.send(msg);
}

} // namespace ipm
} // namespace dunedaq