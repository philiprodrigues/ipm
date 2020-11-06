/**
 *
 * @file ZmqReceiverImpl.hpp Implementations of common routines for ZeroMQ Receivers
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef IPM_PLUGINS_ZMQRECEIVERIMPL_HPP_
#define IPM_PLUGINS_ZMQRECEIVERIMPL_HPP_

#include "TRACE/trace.h"

#include "ipm/Subscriber.hpp"
#include "ipm/ZmqContext.hpp"

#include <string>
#include <vector>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {

// Remember that Subscriber is a superset of Receiver
class ZmqReceiverImpl : public Subscriber
{
public:
  enum class ReceiverType
  {
    Subscriber,
    Pull,
  };

  ZmqReceiverImpl(ReceiverType type)
    : socket_(ZmqContext::instance().GetContext(), type == ReceiverType::Pull ? zmq::socket_type::pull : zmq::socket_type::sub)
  {
  }
  bool can_receive() const noexcept override { return socket_connected_; }
  void connect_for_receives(const nlohmann::json& connection_info) override
  {
    std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
    TLOG(TLVL_INFO) << "Connection String is " << connection_string;
    socket_.connect(connection_string);
    socket_connected_ = true;
  }

  void subscribe(std::string const& topic) override { socket_.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size()); }
  void unsubscribe(std::string const& topic) override
  {
    socket_.setsockopt(ZMQ_UNSUBSCRIBE, topic.c_str(), topic.size());
  }

protected:
  Receiver::Response receive_(const duration_type& timeout) override
  {
    auto timeout_in_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    socket_.setsockopt(ZMQ_RCVTIMEO, timeout_in_ms);
    Receiver::Response output;
    zmq::message_t hdr, msg;
    size_t res = 0;
    try {
      res = socket_.recv(&hdr);
    } catch (zmq::error_t const& err) {
      // Throw ERS-ified exception
    }
    if (res > 0) {
      output.metadata.reserve(hdr.size());
      memcpy(&output.metadata[0], hdr.data(), hdr.size());

      // ZMQ guarantees that the entire message has arrived
      socket_.recv(&msg);
      output.data.reserve(msg.size());
      memcpy(&output.data[0], msg.data(), msg.size());
    } else {
      // timeout
    }
    return output;
  }

private:
  zmq::socket_t socket_;
  bool socket_connected_{ false };
};

} // namespace ipm
} // namespace dunedaq

#endif // IPM_PLUGINS_ZMQRECEIVERIMPL_HPP_