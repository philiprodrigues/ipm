/**
 *
 * @file ZmqSubscriber.cpp ZmqSubscriber messaging class definitions
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TRACE/trace.h"
#define TRACE_NAME "ZmqSubscriber"

#include "ipm/Subscriber.hpp"
#include "ipm/ZmqContext.hpp"

#include <string>
#include <vector>
#include <zmq.hpp>

namespace dunedaq {
namespace ipm {
class ZmqSubscriber : public Subscriber
{
public:
  ZmqSubscriber()
    : socket_(ZmqContext::instance().GetContext(), zmq::socket_type::sub)
  {}
  bool can_receive() const noexcept override;
  void connect_for_receives(const nlohmann::json& connection_info);

  void subscribe(std::string const& topic) override;
  void unsubscribe(std::string const& topic) override;

protected:
  std::vector<char> receive_(const duration_type& timeout, std::string& topic) override;

private:
  zmq::socket_t socket_;
  bool socket_connected_{ false };
};

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
  socket_connected_ = true;
}

void
ZmqSubscriber::subscribe(std::string const& topic)
{
  socket_.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
}

void
ZmqSubscriber::unsubscribe(std::string const& topic)
{
  socket_.setsockopt(ZMQ_UNSUBSCRIBE, topic.c_str(), topic.size());
}

std::vector<char>
ZmqSubscriber::receive_(const duration_type&, std::string& topic)
{
  std::vector<char> output;
  zmq::message_t hdr, msg;
  auto res = socket_.recv(&hdr);
  if (res ) {
    std::string tmptopic;
    tmptopic.reserve(hdr.size());
    memcpy(&tmptopic[0], hdr.data(), hdr.size());

    if (topic != "" && topic != tmptopic) {
    /// TODO: Eric Flumerfelt (29-Oct-2020): Decide what to do here. Options are basically discard or cache
    }
    topic = tmptopic;

    // ZMQ guarantees that the entire message has arrived
    socket_.recv(&msg);
    output = std::vector<char>(msg.size());
    memcpy(&output[0], msg.data(), msg.size());
  }
  return output;
}

} // namespace ipm
} // namespace dunedaq

DEFINE_DUNE_IPM_RECEIVER(dunedaq::ipm::ZmqSubscriber)