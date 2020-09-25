/**
 * @file ipmReceiver.hpp ipmReceiver Class Interface
 *
 * ipmReceiver defines the interface of objects which can receive messages
 * between processes
 *
 * Implementor of this interface is required to:
 *
 * - Implement the protected virtual receive_ function, called by public non-virtual receive
 * - Implement the public virtual can_receive function
 * - Implement the public virtual connect_for_receives function  
 *
 * And is encouraged to:
 *
 * - Meaningfully implement the timeout feature in receive_, and have it
 *   throw the ReceiveTimeoutExpired exception if it occurs
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_INCLUDE_IPM_IPMRECEIVER_HPP_
#define IPM_INCLUDE_IPM_IPMRECEIVER_HPP_

#include "ers/Issue.h"
#include "nlohmann/json.hpp"

#include <vector>

namespace dunedaq {
ERS_DECLARE_ISSUE(ipm, KnownStateForbidsReceive, "Receiver not in a state to send data", )
ERS_DECLARE_ISSUE(ipm,
                  UnexpectedNumberOfBytes,
                  "Expected " << bytes1 << " bytes in message but received " << bytes2,
                  ((int)bytes1)((int)bytes2)) // NOLINT
ERS_DECLARE_ISSUE(ipm,
                  ReceiveTimeoutExpired,
                  "Unable to receive within timeout period (timeout period was " << timeout << " milliseconds)",
                  ((int)timeout)) // NOLINT
} // namespace dunedaq

namespace dunedaq::ipm {

class ipmReceiver
{

public:
  using duration_type = std::chrono::milliseconds;
  static constexpr duration_type block = std::chrono::duration_values<duration_type>::max();
  static constexpr duration_type noblock = std::chrono::duration_values<duration_type>::zero();

  using size_type = int;
  static constexpr size_type anysize =
    0; // Since "I want 0 bytes" is pointless, "0" denotes "I don't care about the size"

  ipmReceiver() = default;

  virtual void connect_for_receives(const nlohmann::json& connection_info) = 0; 

  virtual bool can_receive() const noexcept = 0;

  // receive() will perform some universally-desirable checks before calling user-implemented receive_:
  // -Throws KnownStateForbidsReceive if can_receive() == false
  // -Throws UnexpectedNumberOfBytes if the "nbytes" argument isn't anysize, and the
  //  received bytes inside the function aren't the same number as nbytes

  std::vector<char> receive(const duration_type& timeout, size_type nbytes = anysize);

  // Is it worth also implementing a receive_multipart where you tell
  // the function the various sizes of each message you expect?

  std::vector<std::vector<char>> receive_multipart(const duration_type& timeout_per_message, size_type n_messages)
  {
    std::vector<std::vector<char>> messages;

    for (size_type i_message = 0; i_message < n_messages; ++i_message) {
      messages.emplace_back(receive(timeout_per_message));
    }
    return messages;
  }

  ipmReceiver(const ipmReceiver&) = delete;
  ipmReceiver& operator=(const ipmReceiver&) = delete;

  ipmReceiver(ipmReceiver&&) = delete;
  ipmReceiver& operator=(ipmReceiver&&) = delete;

protected:
  virtual std::vector<char> receive_(const duration_type& timeout) = 0;
};

inline std::vector<char>
ipmReceiver::receive(const duration_type& timeout, size_type bytes)
{
  if (!can_receive()) {
    throw KnownStateForbidsReceive(ERS_HERE);
  }
  std::vector<char> message = receive_(timeout);

  if (bytes != anysize) {
    auto received_size = static_cast<size_type>(message.size());
    if (received_size != bytes) {
      throw UnexpectedNumberOfBytes(ERS_HERE, received_size, bytes);
    }
  }

  return message;
}

} // namespace dunedaq::ipm

#endif // IPM_INCLUDE_IPM_IPMRECEIVER_HPP_
