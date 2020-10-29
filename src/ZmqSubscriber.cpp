/**
 *
 * @file ZmqSubscriber.cpp ZmqSubscriber messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ZmqSubscriber.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqSubscriber"
#include <string>
#include <vector>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {

bool
ZmqSubscriber::can_receive() const noexcept
{
  return socket_connected_;
}

void
ZmqSubscriber::connect_for_receives(const nlohmann::json& connection_info)
{
  std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
  TLOG(TLVL_INFO) << "Connection String is " << connection_string;
  socket_.connect(connection_string);
  socket_.setsockopt(ZMQ_SUBSCRIBE, topic_.c_str(), topic_.size());
  socket_connected_ = true;
}

std::vector<char>
ZmqSubscriber::receive_(const duration_type&)
{
  std::vector<char> output;
  zmq::message_t hdr, msg;
  auto res = socket_.recv(&hdr);
  if (res && hdr.size() == topic_.size() && std::string(static_cast<char*>(hdr.data())) == topic_) {
    // ZMQ guarantees that the entire message has arrived
    socket_.recv(&msg);
    output = std::vector<char>(msg.size());
    memcpy(&output[0], msg.data(), msg.size());
  }
  return output;
}

} // namespace ipm
} // namespace dunedaq