/**
 * @file Sender.hpp Sender Class Interface
 *
 * Sender defines the interface of objects which can send messages
 * between processes
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

#ifndef IPM_INCLUDE_IPM_SENDER_HPP_
#define IPM_INCLUDE_IPM_SENDER_HPP_

#include "ers/Issue.h"
#include "nlohmann/json.hpp"

#include <cetlib/BasicPluginFactory.h>
#include <cetlib/compiler_macros.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
ERS_DECLARE_ISSUE(ipm, KnownStateForbidsSend, "Sender not in a state to send data", )
ERS_DECLARE_ISSUE(ipm, NullPointerPassedToSend, "An null pointer to memory was passed to Sender::send", )
ERS_DECLARE_ISSUE(ipm,
                  SendTimeoutExpired,
                  "Unable to send within timeout period (timeout period was " << timeout << " milliseconds)",
                  ((int)timeout)) // NOLINT

} // namespace dunedaq

#ifndef EXTERN_C_FUNC_DECLARE_START
#define EXTERN_C_FUNC_DECLARE_START                                                                                    \
  extern "C"                                                                                                           \
  {
#endif

/**
 * @brief Declare the function that will be called by the plugin loader
 * @param klass Class to be defined as a DUNE IPM Sender
 */
#define DEFINE_DUNE_IPM_SENDER(klass)                                                                                  \
  EXTERN_C_FUNC_DECLARE_START                                                                                          \
  std::shared_ptr<dunedaq::ipm::Sender> make() { return std::shared_ptr<dunedaq::ipm::Sender>(new klass()); }          \
  }

namespace dunedaq::ipm {

class Sender
{

public:
  using duration_type = std::chrono::milliseconds;
  static constexpr duration_type block = std::chrono::duration_values<duration_type>::max();
  static constexpr duration_type noblock = std::chrono::duration_values<duration_type>::zero();

  using size_type = int;

  Sender() = default;

  virtual void connect_for_sends(const nlohmann::json& connection_info) = 0;

  virtual bool can_send() const noexcept = 0;

  // send() will perform some universally-desirable checks before calling user-implemented send_()
  // -Throws KnownStateForbidsSend if can_send() == false
  // -Throws NullPointerPassedToSend if message is a null pointer
  // -If message_size == 0, function is a no-op

  void send(const void* message,
            size_type message_size,
            const duration_type& timeout,
            std::string const& metadata = "");

  void send_multipart(const void** message_parts,
                      const std::vector<size_type>& message_sizes,
                      const duration_type& timeout,
                      std::string const& metadata = "");

  Sender(const Sender&) = delete;
  Sender& operator=(const Sender&) = delete;

  Sender(Sender&&) = delete;
  Sender& operator=(Sender&&) = delete;

protected:
  virtual void send_(const void* message, size_type N, const duration_type& timeout, std::string const& metadata) = 0;
  virtual void send_multipart_(const void** message_parts,
                               const std::vector<size_type>& message_sizes,
                               const duration_type& timeout,
                               std::string const& metadata)
  {
    for (size_t i = 0; i < message_sizes.size(); ++i) {
      send_(message_parts[i], message_sizes[i], timeout, metadata);
    }
  }
};

inline void
Sender::send(const void* message, size_type message_size, const duration_type& timeout, std::string const& metadata)
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

  send_(message, message_size, timeout, metadata);
}

inline void
Sender::send_multipart(const void** message_parts,
                       const std::vector<size_type>& message_sizes,
                       const duration_type& timeout,
                       std::string const& metadata)
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

  send_multipart_(message_parts, message_sizes, timeout, metadata);
}

std::shared_ptr<Sender>
makeIPMSender(std::string const& plugin_name)
{
  static cet::BasicPluginFactory bpf("duneIPM", "make");
  return bpf.makePlugin<std::shared_ptr<Sender>>(plugin_name);
}

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_SENDER_HPP_
