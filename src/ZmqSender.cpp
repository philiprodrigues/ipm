/**
 *
 * @file ZmqSender.cpp ZmqSender messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ZmqSender.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqSender"
#include <string>

namespace dunedaq {
namespace ipm {

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
ZmqSender::send_(const void* message, int N, const duration_type& )
{
  zmq::message_t msg(message, N);
  socket_.send(msg);
}

} // namespace ipm
} // namespace dunedaq