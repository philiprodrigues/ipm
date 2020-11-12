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

  explicit ZmqReceiverImpl(ReceiverType type)
    : socket_(ZmqContext::instance().GetContext(),
              type == ReceiverType::Pull ? zmq::socket_type::pull : zmq::socket_type::sub)
  {}
  bool can_receive() const noexcept override { return socket_connected_; }
  void connect_for_receives(const nlohmann::json& connection_info) override
  {
    std::string connection_string = connection_info.value<std::string>("connection_string", "inproc://default");
    TLOG(TLVL_INFO) << "Connection String is " << connection_string;
    socket_.setsockopt(ZMQ_RCVTIMEO, 1); // 1 ms, we'll repeat until we reach timeout
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
    Receiver::Response output;
    zmq::message_t hdr, msg;
    size_t res = 0;

    auto start_time = std::chrono::steady_clock::now();
    do {

      try {
        TLOG(TLVL_TRACE + 3) << "Going to receive header";
        res = socket_.recv(&hdr);
        TLOG(TLVL_TRACE + 3) << "Recv res=" << res << " for header (hdr.size() == " << hdr.size() << ")";
      } catch (zmq::error_t const& err) {
        // Throw ERS-ified exception
      }
      if (res > 0 || hdr.more()) {
        TLOG(TLVL_TRACE + 3) << "Going to receive data";
        output.metadata.resize(hdr.size());
        memcpy(&output.metadata[0], hdr.data(), hdr.size());

        // ZMQ guarantees that the entire message has arrived
        res = socket_.recv(&msg);
        TLOG(TLVL_TRACE + 3) << "Recv res=" << res << " for data (msg.size() == " << msg.size() << ")";
        output.data.resize(msg.size());
        memcpy(&output.data[0], msg.data(), msg.size());
      } else {
        usleep(1000);
      }
    } while (std::chrono::steady_clock::now() - start_time < timeout && res == 0);

    #if 0 /// TODO, ELF 11/12/2020: Why doesn't this compile?
    if (res == 0) {
        throw ReceiveTimeoutExpired(ERS_HERE);
    }
    #endif

    TLOG(TLVL_TRACE + 2) << "Returning output with metadata size " << output.metadata.size() << " and data size "
                    << output.data.size();
    return output;
  }

private:
  zmq::socket_t socket_;
  bool socket_connected_{ false };
};

} // namespace ipm
} // namespace dunedaq

#endif // IPM_PLUGINS_ZMQRECEIVERIMPL_HPP_
