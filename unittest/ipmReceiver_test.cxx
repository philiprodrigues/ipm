/**
 * @file ipmReceiver_test.cxx ipmReceiver class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/ipmReceiver.hpp"

#define BOOST_TEST_MODULE ipmReceiver_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(ipmReceiver_test)

namespace {

class ipmReceiverImpl : public ipmReceiver
{

public:
  static const size_type bytesOnEachReceive = 10;

  ipmReceiverImpl()
    : can_receive_(false)
  {}

  void connect_for_receives(const nlohmann::json& /* connection_info */) {}
  bool can_receive() const noexcept override { return can_receive_; }
  void make_me_ready_to_receive() { can_receive_ = true; }
  void sabotage_my_receiving_ability() { can_receive_ = false; }

protected:
  std::vector<char> receive_(const duration_type& /* timeout */) override
  {
    return std::vector<char>(bytesOnEachReceive, 'A');
  }

private:
  bool can_receive_;
};

} // namespace ""

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<ipmReceiverImpl>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<ipmReceiverImpl>);
  BOOST_REQUIRE(!std::is_move_constructible_v<ipmReceiverImpl>);
  BOOST_REQUIRE(!std::is_move_assignable_v<ipmReceiverImpl>);
}

BOOST_AUTO_TEST_CASE(StatusChecks)
{
  ipmReceiverImpl theReceiver;

  BOOST_REQUIRE(!theReceiver.can_receive());

  theReceiver.make_me_ready_to_receive();
  BOOST_REQUIRE(theReceiver.can_receive());

  BOOST_REQUIRE_NO_THROW(theReceiver.receive(ipmReceiver::noblock));
  BOOST_REQUIRE_NO_THROW(theReceiver.receive(ipmReceiver::noblock, ipmReceiverImpl::bytesOnEachReceive));

  BOOST_REQUIRE_EXCEPTION(theReceiver.receive(ipmReceiver::noblock, ipmReceiverImpl::bytesOnEachReceive - 1),
                          dunedaq::ipm::UnexpectedNumberOfBytes,
                          [&](dunedaq::ipm::UnexpectedNumberOfBytes) { return true; });

  theReceiver.sabotage_my_receiving_ability();
  BOOST_REQUIRE(!theReceiver.can_receive());

  BOOST_REQUIRE_EXCEPTION(theReceiver.receive(ipmReceiver::noblock),
                          dunedaq::ipm::KnownStateForbidsReceive,
                          [&](dunedaq::ipm::KnownStateForbidsReceive) { return true; });
}

BOOST_AUTO_TEST_SUITE_END()
