/**
 *
 * @file ZmqSender.cpp ZmqSender messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqSender"

#include "ipm/Sender.hpp"
#include "ipm/ZmqContext.hpp"

#include <string>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqSender : public Sender
{
public:
  ZmqSender()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::push)
  {}
  bool can_send() const noexcept override;
  void connect_for_sends(const nlohmann::json& connection_info);

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
ZmqSender::connect_for_sends(const nlohmann::json& connection_info)
{
  std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
  TLOG(TLVL_INFO) << "Connection String is " << connection_string;
  socket_.connect(connection_string);
  socket_connected_ = true;
}

void
ZmqSender::send_(const void* message, int N, const duration_type&)
{
  zmq::message_t msg(message, N);
  socket_.send(msg);
}

} // namespace ipm
} // namespace dunedaq

DEFINE_DUNE_IPM_SENDER(dunedaq::ipm::ZmqSender)