/**
 *
 * @file ZmqPublisher.cpp ZmqPublisher messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqPublisher"
#include <string>

#include "ipm/Publisher.hpp"
#include "ipm/ZmqContext.hpp"

#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqPublisher : public Publisher
{
public:
  ZmqPublisher()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::pub)
  {}
  bool can_send() const noexcept override;
  void connect_for_sends(const nlohmann::json& connection_info);

protected:
  void publish_(const void* message, int N, const duration_type& timeout, std::string const& topic) override;

private:
  zmq::socket_t socket_;
  bool socket_connected_;
};

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
ZmqPublisher::publish_(const void* message, int N, const duration_type&, std::string const& topic)
{
  zmq::message_t topic_msg(topic.c_str(), topic.size());
  zmq::message_t msg(message, N);
  socket_.send(topic_msg, ZMQ_SNDMORE);
  socket_.send(msg);
}

} // namespace ipm
} // namespace dunedaq

DEFINE_DUNE_IPM_SENDER(dunedaq::ipm::ZmqPublisher)