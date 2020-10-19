/**
 *
 * @file ZmqReceiver.cpp ZmqReceiver messaging class declarations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_INCLUDE_IPM_ZMQRECEIVER_HPP_
#define IPM_INCLUDE_IPM_ZMQRECEIVER_HPP_

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
  void connect_for_receives(const nlohmann::json& connection_info);

protected:
  std::vector<char> receive_(const duration_type& timeout) override;

private:
  zmq::socket_t socket_;
  bool socket_connected_{ false };
};

} // namespace ipm
} // namespace dunedaq

#endif // IPM_INCLUDE_IPM_ZMQRECEIVER_HPP_