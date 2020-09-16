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

  ipmSender() = default;

  virtual bool can_send() const noexcept = 0;

  void send(const char* message, int message_size)
  {
    if (!can_send()) {
      throw KnownStateForbidsSend(ERS_HERE);
    }

    if (!message) {
      throw NullPointerPassedToSend(ERS_HERE);
    }

    send_(message, message_size);
  }

  void send(const char** message_parts, const std::vector<int>& message_sizes)
  {

    for (size_t i = 0; i < message_sizes.size(); ++i) {
      send(message_parts[i], message_sizes[i]);
    }
  }

  ipmSender(const ipmSender&) = delete;
  ipmSender& operator=(const ipmSender&) = delete;

  ipmSender(ipmSender&&) = delete;
  ipmSender& operator=(ipmSender&&) = delete;

protected:
  // send_ is the heart of the interface
  virtual void send_(const char* message, int N) = 0;
};

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_IPMSENDER_HPP_
