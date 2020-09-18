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
 * And is encouraged to:
 *
 * - Meaningfully implement the timeout feature in send_, and have it
 *   throw the SendTimeoutExpired exception if it occurs
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_INCLUDE_IPM_IPMSENDER_HPP_
#define IPM_INCLUDE_IPM_IPMSENDER_HPP_

#include "ers/Issue.h"
#include <string>
#include <vector>

namespace dunedaq {
ERS_DECLARE_ISSUE(ipm, KnownStateForbidsSend, "Sender not in a state to send data", )
ERS_DECLARE_ISSUE(ipm, NullPointerPassedToSend, "An null pointer to memory was passed to ipmSender::send", )
ERS_DECLARE_ISSUE(ipm,
                  SendTimeoutExpired,
                  "Unable to send within timeout period (timeout period was " << timeout << " milliseconds)",
                  ((int)timeout)) // NOLINT

} // namespace dunedaq

namespace dunedaq::ipm {

class ipmSender
{

public:
  using duration_type = std::chrono::milliseconds;
  static constexpr duration_type block = std::chrono::duration_values<duration_type>::max();
  static constexpr duration_type noblock = std::chrono::duration_values<duration_type>::zero();

  using size_type = int;

  ipmSender() = default;

  virtual bool can_send() const noexcept = 0;

  // send() will perform some universally-desirable checks before calling user-implemented send_()
  // -Throws KnownStateForbidsSend if can_send() == false
  // -Throws NullPointerPassedToSend if message is a null pointer
  // -If message_size == 0, function is a no-op

  void send(const char* message, size_type message_size, const duration_type& timeout);

  void send_multipart(const char** message_parts, const std::vector<size_type>& message_sizes, const duration_type& timeout)
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
  virtual void send_(const char* message, size_type N, const duration_type& timeout) = 0;
};

inline void
ipmSender::send(const char* message, size_type message_size, const duration_type& timeout)
{
  if (message_size == 0) {
    return;
  }

  if (!can_send()) {
    throw KnownStateForbidsSend(ERS_HERE);
  }

  if (!message) {
    throw NullPointerPassedToSend(ERS_HERE);
  }

  send_(message, message_size, timeout);
}

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_IPMSENDER_HPP_
