#ifndef IPM_INCLUDE_IPM_SUBSCRIBER_HPP_
#define IPM_INCLUDE_IPM_SUBSCRIBER_HPP_

/**
 *
 * @file Subscriber.hpp Subscriber messaging role class declarations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmReceiver.hpp"

#include <string>
#include <vector>

namespace dunedaq::ipm {

class Subscriber : public ipmReceiver
{
public:
  bool can_receive() const noexcept override { return false; }

  void reset_topics(const std::string& /* topics */) {}
  std::string get_topics() const { return "If you can see this, Subscriber::get_topics is not implemented"; }
  void add_topic(const std::string& /* topic */) {}
  void remove_topic(const std::string& /* topic */) {}

protected:
  std::vector<char> receive_(const duration_type& /* timeout */) override { return std::vector<char>(); }
};
} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_SUBSCRIBER_HPP_
