/**
 *
 * @file ZmqReceiver.cpp ZmqReceiver messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ZmqReceiver.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqReceiver"
#include <zmq.hpp>
#include <vector>
#include <string>

namespace dunedaq {
namespace ipm {

bool
ZmqReceiver::can_receive() const noexcept
{
  return socket_connected_;
}

void
ZmqReceiver::connect_for_receives(const nlohmann::json& connection_info)
{
  std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
  TLOG(TLVL_INFO) << "Connection String is " << connection_string;
  socket_.bind(connection_string);
  socket_connected_ = true;
}

std::vector<char>
ZmqReceiver::receive_(const duration_type&)
{
  std::vector<char> output;
  zmq::message_t msg;
  auto res = socket_.recv(&msg);
  if (res) {
    output = std::vector<char>(msg.size());
    memcpy(&output[0], msg.data(), msg.size());
  }
  return output;
}

} // namespace ipm
} // namespace dunedaq