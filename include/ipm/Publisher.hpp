#ifndef IPM_INCLUDE_IPM_PUBLISHER_HPP_
#define IPM_INCLUDE_IPM_PUBLISHER_HPP_

/**
 *
 * @file Publisher.hpp Publisher messaging role class declarations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmSender.hpp"

namespace dunedaq::ipm {
  class Publisher : public ipmSender
  {
  public:
    bool can_send() const noexcept override { return false; }

  protected:
    void send_(const char* /* message */, int /* N */, const duration_type& /* timeout */) override {}
  };
} // namespace dunedaq::ipm


#endif // IPM_INCLUDE_IPM_PUBLISHER_HPP_
