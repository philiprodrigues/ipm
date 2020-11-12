/**
 *
 * @file ZmqSenderImpl.cpp Implementations of common routines for ZeroMQ Senders
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_PLUGINS_ZMQSENDERIMPL_HPP_
#define IPM_PLUGINS_ZMQSENDERIMPL_HPP_

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

  explicit ZmqSenderImpl(SenderType type)
    : socket_(ZmqContext::instance().GetContext(),
              type == SenderType::Push ? zmq::socket_type::push : zmq::socket_type::pub)
  {}
  bool can_send() const noexcept override { return socket_connected_; }
  void connect_for_sends(const nlohmann::json& connection_info)
  {
    std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
    TLOG(TLVL_INFO) << "Connection String is " << connection_string;
    socket_.setsockopt(ZMQ_SNDTIMEO, 1); // 1 ms, we'll repeat until we reach timeout
    socket_.bind(connection_string);
    socket_connected_ = true;
  }

protected:
  void send_(const void* message, int N, const duration_type& timeout, std::string const& topic) override
  {
    TLOG(TLVL_INFO) << "Starting send of " << N << " bytes";
    auto start_time = std::chrono::steady_clock::now();
    bool res = false;
    do {

      zmq::message_t topic_msg(topic.c_str(), topic.size());
      res = socket_.send(topic_msg, ZMQ_SNDMORE);

      if (!res) {
        TLOG(TLVL_INFO) << "Unable to send message";
        continue;
      }

      zmq::message_t msg(message, N);
      res = socket_.send(msg);
    } while (std::chrono::steady_clock::now() - start_time < timeout && !res);

#if 0 /// TODO, ELF 11/12/2020: Why doesn't this compile?
    if (!res) {
      throw SendTimeoutExpired(ERS_HERE);
    }
    #endif

    TLOG(TLVL_INFO) << "Completed send of " << N << " bytes";
  }

private:
  zmq::socket_t socket_;
  bool socket_connected_;
};

} // namespace ipm
} // namespace dunedaq

#endif // IPM_PLUGINS_ZMQSENDERIMPL_HPP_