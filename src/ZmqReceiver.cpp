/**
 *
 * @file ZmqReceiver.cpp ZmqReceiver messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ZmqContext.hpp"
#include "ipm/ipmReceiver.hpp"

#include <zmq.hpp>
#include <vector>

namespace dunedaq {
namespace ipm {
class ZmqReceiver : public ipmReceiver
{
public:
  ZmqReceiver()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::pull)
  {}
  bool can_receive() const noexcept override;

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