/**
 *
 * @file ZmqSenderImpl.cpp Implementations of common routines for ZeroMQ Senders
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */


#include "TRACE/trace.h"

#include "ipm/Sender.hpp"
#include "ipm/ZmqContext.hpp"

#include <string>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqSenderImpl : public Sender
{
public:
  enum class SenderType
  {
  Publisher,
  Push,
  };

  ZmqSenderImpl(SenderType type)
    : socket_(ZmqContext::instance().GetContext(), type == SenderType::Push ? zmq::socket_type::push : zmq::socket_type::pub)
  {
  }
  bool can_send() const noexcept override { return socket_connected_; }
  void connect_for_sends(const nlohmann::json& connection_info)
  {
    std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
    TLOG(TLVL_INFO) << "Connection String is " << connection_string;
    socket_.bind(connection_string);
    socket_connected_ = true;
  }

protected:
  void send_(const void* message, int N, const duration_type& timeout, std::string const& topic) override
  {
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    zmq::message_t msg(message, N);
    socket_.send(topic_msg, ZMQ_SNDMORE);
    socket_.send(msg);
  }

private:
  zmq::socket_t socket_;
  bool socket_connected_;
};


} // namespace ipm
} // namespace dunedaq
