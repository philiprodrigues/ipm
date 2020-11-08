/**
 * @file ZmqSubscriber_test.cxx ZmqSubscriber class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Subscriber.hpp"

#define BOOST_TEST_MODULE ZmqSubscriber_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(ZmqSubscriber_test)

BOOST_AUTO_TEST_CASE(BasicTests)
{
  auto theSubscriber = makeIPMSubscriber("ZmqSubscriber");
  BOOST_REQUIRE(theSubscriber != nullptr);
  BOOST_REQUIRE(!theSubscriber->can_receive());

  auto theReceiver = makeIPMReceiver("ZmqSubscriber");
  BOOST_REQUIRE(theReceiver != nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
