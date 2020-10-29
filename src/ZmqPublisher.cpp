/**
 *
 * @file ZmqPublisher.cpp ZmqPublisher messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ZmqPublisher.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqPublisher"
#include <string>

namespace dunedaq {
namespace ipm {

bool
ZmqPublisher::can_send() const noexcept
{
  return socket_connected_;
}

void
ZmqPublisher::connect_for_sends(const nlohmann::json& connection_info)
{
  std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
  TLOG(TLVL_INFO) << "Connection String is " << connection_string;
  socket_.bind(connection_string);
  socket_connected_ = true;
}

void
ZmqPublisher::send_(const void* message, int N, const duration_type& )
{
  zmq::message_t topic(topic_.c_str(), topic_.size());
  zmq::message_t msg(message, N);
  socket_.send(topic, ZMQ_SNDMORE);
  socket_.send(msg);
}

} // namespace ipm
} // namespace dunedaq