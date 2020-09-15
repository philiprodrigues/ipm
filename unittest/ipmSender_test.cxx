/**
 * @file ipmSender_test.cxx ipmSender class Unit Tests
 *
 * This file is meant to serve as an example for developers for how to use Boost
 * to write unit tests for their package's components. It's good practice to write
 * unit tests and make sure they pass before passing code on to others.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmSender.hpp"

#define BOOST_TEST_MODULE ipmSender_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(ipmSender_test)

namespace {

  class ipmSenderImpl: public ipmSender {

  public:
    bool CheckImUninitialized() { return GetSenderStatus() == SenderStatus::kUninitialized; }
    void MakeMeReadyToSend() { SetSenderStatus(SenderStatus::kWasEnabled); }
    void SabotageMySendingAbility() {SetSenderStatus(SenderStatus::kFoundInError); }

  protected:
    void send_(const char* /* message */, int /* N */ ) override {
      // Pretty unexciting stub
    }
    
  };

} // namespace ""

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE( !std::is_copy_constructible_v<ipmSenderImpl> );
  BOOST_REQUIRE( !std::is_copy_assignable_v<ipmSenderImpl> );
  BOOST_REQUIRE( !std::is_move_constructible_v<ipmSenderImpl> );
  BOOST_REQUIRE( !std::is_move_assignable_v<ipmSenderImpl> );
}

BOOST_AUTO_TEST_CASE(StatusChecks)
{
  ipmSenderImpl theSender;
  std::vector<char> random_data { 'T', 'E', 'S', 'T' };

  BOOST_REQUIRE(theSender.CheckImUninitialized());

  theSender.MakeMeReadyToSend();
  BOOST_REQUIRE(theSender.ReadyToSend());

  BOOST_REQUIRE_NO_THROW(theSender.send(random_data.data(), random_data.size()));

  theSender.SabotageMySendingAbility();
  BOOST_REQUIRE(!theSender.ReadyToSend());

  BOOST_REQUIRE_EXCEPTION(theSender.send(random_data.data(), random_data.size()), dunedaq::ipm::KnownStateForbidsSend, [&](dunedaq::ipm::KnownStateForbidsSend) { return true; });
  
}

BOOST_AUTO_TEST_CASE(BadInput)
{
  ipmSenderImpl theSender;
  theSender.MakeMeReadyToSend();
  
  const char* badBytes = nullptr;
  BOOST_REQUIRE_EXCEPTION(theSender.send(badBytes, 10), dunedaq::ipm::NullPointerPassedToSend, [&](dunedaq::ipm::NullPointerPassedToSend) { return true; });

}

BOOST_AUTO_TEST_SUITE_END()
