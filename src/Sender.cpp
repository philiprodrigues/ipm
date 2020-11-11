/**
 * @file Sender.hpp Sender Class implementations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Sender.hpp"

void
dunedaq::ipm::Sender::send(const void* message, size_type message_size, const duration_type& timeout, std::string const& metadata)
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

void
dunedaq::ipm::Sender::send_multipart(const void** message_parts,
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
