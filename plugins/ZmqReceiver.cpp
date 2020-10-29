/**
 *
 * @file ZmqReceiver.cpp ZmqReceiver messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqReceiver"

#include "ipm/Receiver.hpp"
#include "ipm/ZmqContext.hpp"

#include <string>
#include <vector>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqReceiver : public Receiver
{
public:
  ZmqReceiver()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::pull)
  {}
  bool can_receive() const noexcept override;
  void connect_for_receives(const nlohmann::json& connection_info);

protected:
  std::vector<char> receive_(const duration_type& timeout) override;

private:
  zmq::socket_t socket_;
  bool socket_connected_{ false };
};

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

DEFINE_DUNE_IPM_RECEIVER(dunedaq::ipm::ZmqReceiver)