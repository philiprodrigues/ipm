/**
 * @file ipmSender.hpp ipmSender Class Interface
 *
 * ipmSender defines the interface of objects which can send messages
 * between processes
 *
 * Implementor of this interface is required to:
 *
 * - Implement the protected virtual send_ function, called by public non-virtual send
 * - Implement the public virtual can_send function
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_INCLUDE_IPM_IPMSENDER_HPP_
#define IPM_INCLUDE_IPM_IPMSENDER_HPP_

#include <string>

#include "ers/Issue.h"

namespace dunedaq {
ERS_DECLARE_ISSUE(ipm, KnownStateForbidsSend, "Sender not in a state to send data", )
ERS_DECLARE_ISSUE(ipm, NullPointerPassedToSend, "An null pointer to memory was passed to ipmSender::send", )
} // namespace dunedaq

namespace dunedaq::ipm {

class ipmSender
{

public:

  using duration_type = std::chrono::milliseconds; 
  static constexpr duration_type block =   std::chrono::duration_values<duration_type>::max();
  static constexpr duration_type noblock = std::chrono::duration_values<duration_type>::zero();
  
  ipmSender() = default;

  virtual bool can_send() const noexcept = 0;

  void send(const char* message, int message_size, const duration_type& timeout)
  {
    if (!can_send()) {
      throw KnownStateForbidsSend(ERS_HERE);
    }

    if (!message) {
      throw NullPointerPassedToSend(ERS_HERE);
    }

    send_(message, message_size, timeout);
  }

  void send(const char** message_parts, const std::vector<int>& message_sizes, const duration_type& timeout)
  {

    for (size_t i = 0; i < message_sizes.size(); ++i) {
      send(message_parts[i], message_sizes[i], timeout);
    }
  }

  ipmSender(const ipmSender&) = delete;
  ipmSender& operator=(const ipmSender&) = delete;

  ipmSender(ipmSender&&) = delete;
  ipmSender& operator=(ipmSender&&) = delete;

protected:
  // send_ is the heart of the interface
  virtual void send_(const char* message, int N, const duration_type& timeout) = 0;
};

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_IPMSENDER_HPP_