/**
 * @file Receiver.hpp Receiver Class implementations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Receiver.hpp"

dunedaq::ipm::Receiver::Response
dunedaq::ipm::Receiver::receive(const duration_type& timeout, size_type bytes)
{
  if (!can_receive()) {
    throw KnownStateForbidsReceive(ERS_HERE);
  }
  auto message = receive_(timeout);

  if (bytes != anysize) {
    auto received_size = static_cast<size_type>(message.data.size());
    if (received_size != bytes) {
      throw UnexpectedNumberOfBytes(ERS_HERE, received_size, bytes);
    }
  }

  return message;
}
