/**
 * @file Publisher.hpp Publisher Class Interface
 *
 * Publisher defines the interface of objects which can send messages
 * between processes on various topics. It also is a Sender, using the default topic
 *
 * Implementor of this interface is required to:
 *
 * - Implement the protected virtual send_ function, called by public non-virtual send
 * - Implement the public virtual can_send function
 * - Implement the public virtual connect_for_sends function
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

#ifndef IPM_INCLUDE_IPM_PUBLISHER_HPP_
#define IPM_INCLUDE_IPM_PUBLISHER_HPP_

#include "ipm/Sender.hpp"

#include "ers/Issue.h"
#include "nlohmann/json.hpp"

#include <cetlib/BasicPluginFactory.h>
#include <cetlib/compiler_macros.h>

#include <string>
#include <vector>

namespace dunedaq::ipm {

class Publisher : public Sender
{

public:
  using duration_type = std::chrono::milliseconds;
  static constexpr duration_type block = std::chrono::duration_values<duration_type>::max();
  static constexpr duration_type noblock = std::chrono::duration_values<duration_type>::zero();

  using size_type = int;

  Publisher() = default;

  // send() will perform some universally-desirable checks before calling user-implemented send_()
  // -Throws KnownStateForbidsSend if can_send() == false
  // -Throws NullPointerPassedToSend if message is a null pointer
  // -If message_size == 0, function is a no-op

  void publish(const void* message, size_type message_size, const duration_type& timeout, std::string const& topic);

  void publish_multipart(const void** message_parts,
                         const std::vector<size_type>& message_sizes,
                         const duration_type& timeout,
                         std::string const& topic);

  Publisher(const Publisher&) = delete;
  Publisher& operator=(const Publisher&) = delete;

  Publisher(Publisher&&) = delete;
  Publisher& operator=(Publisher&&) = delete;

protected:
  virtual void publish_(const void* message, size_type N, const duration_type& timeout, std::string const& topic) = 0;
  virtual void publish_multipart_(const void** message_parts,
                                  const std::vector<size_type>& message_sizes,
                                  const duration_type& timeout,
                                  std::string const& topic)
  {
    for (size_t i = 0; i < message_sizes.size(); ++i) {
      publish_(message_parts[i], message_sizes[i], timeout, topic);
    }
  };

  void send_(const void* message, size_type N, const duration_type& timeout) final;
};

inline void
Publisher::publish(const void* message, size_type message_size, const duration_type& timeout, std::string const& topic)
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

  publish_(message, message_size, timeout, topic);
}

inline void
Publisher::publish_multipart(const void** message_parts,
                             const std::vector<size_type>& message_sizes,
                             const duration_type& timeout,
                             std::string const& topic)
{
  if (message_sizes.empty()) {
    return;
  }

  if (!can_send()) {
    throw KnownStateForbidsSend(ERS_HERE);
  }

  if (!message_parts) {
    throw NullPointerPassedToSend(ERS_HERE);
  }

  publish_multipart_(message_parts, message_sizes, timeout, topic);
}

inline void
Publisher::send_(const void* message, size_type message_size, const duration_type& timeout)
{
  publish_(message, message_size, timeout, "IPM");
}

std::shared_ptr<Publisher>
makeIPMPublisher(std::string const& plugin_name)
{
  static cet::BasicPluginFactory bpf("duneIPM", "make");
  auto sender_ptr = bpf.makePlugin<std::shared_ptr<Sender>>(plugin_name);
  return std::dynamic_pointer_cast<Publisher>(sender_ptr);
}

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_PUBLISHER_HPP_
