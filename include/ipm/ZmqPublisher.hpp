/**
 *
 * @file ZmqPublisher.hpp ZmqPublisher messaging class declarations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_INCLUDE_IPM_ZMQPUBLISHER_HPP_
#define IPM_INCLUDE_IPM_ZMQPUBLISHER_HPP_

#include "ipm/ipmSender.hpp"
#include "ipm/ZmqContext.hpp"

#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqPublisher : public ipmSender
{
public:
  ZmqPublisher(std::string const& topic)
    : topic_(topic), socket_(ZmqContext::instance().GetContext(), zmq::socket_type::pub)
  {}
  bool can_send() const noexcept override;
  void connect_for_sends(const nlohmann::json& connection_info);

protected:
  void send_(const void* message, int N, const duration_type& timeout) override;

private:
  std::string topic_;
  zmq::socket_t socket_;
  bool socket_connected_;
};

} // namespace ipm
} // namespace dunedaq

#endif // IPM_INCLUDE_IPM_ZMQPUBLISHER_HPP_