/**
 * @file ZmqReceiver_test.cxx ZmqReceiver class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Receiver.hpp"

#define BOOST_TEST_MODULE ZmqReceiver_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(ZmqReceiver_test)

BOOST_AUTO_TEST_CASE(BasicTests)
{
  auto theReceiver = makeIPMReceiver("ZmqReceiver");
  BOOST_REQUIRE(theReceiver != nullptr);
  BOOST_REQUIRE(!theReceiver->can_receive());
}

BOOST_AUTO_TEST_SUITE_END()
