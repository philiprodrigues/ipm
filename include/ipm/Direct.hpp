#ifndef IPM_INCLUDE_IPM_DIRECT_HPP_
#define IPM_INCLUDE_IPM_DIRECT_HPP_

/**
 *
 * @file Direct.hpp Direct messaging class declarations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmReceiver.hpp"
#include "ipm/ipmSender.hpp"

#include <vector>

namespace dunedaq::ipm {
class Direct
  : public ipmSender
  , public ipmReceiver
{
public:
  virtual void connect_for_sends(const nlohmann::json& /* connection_info */) override {}
  virtual void connect_for_receives(const nlohmann::json& /* connection info */) override {}
  bool can_send() const noexcept override { return false; }
  bool can_receive() const noexcept override { return false; }

protected:
  void send_(const void* /* message */, int /* N */, const ipmSender::duration_type& /* timeout */) override {}
  std::vector<char> receive_(const ipmReceiver::duration_type& /* timeout */) override { return std::vector<char>(); }
};
} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_DIRECT_HPP_
