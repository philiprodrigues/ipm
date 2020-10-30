/**
 * @file Subscriber_test.cxx Subscriber class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Subscriber.hpp"

#define BOOST_TEST_MODULE Subscriber_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(Subscriber_test)

namespace {

class SubscriberImpl : public Subscriber
{

public:
  static const size_type bytesOnEachReceive = 10;

  SubscriberImpl()
    : can_receive_(false)
    , subscriptions_()
  {}

  void connect_for_subscribes(const nlohmann::json& /* connection_info */) {}
  bool can_receive() const noexcept override { return can_receive_; }
  void make_me_ready_to_receive() { can_receive_ = true; }
  void sabotage_my_receiving_ability() { can_receive_ = false; }

  void subscribe(std::string const& topic) override { subscriptions_.insert(topic); }
  void unsubscribe(std::string const& topic) override { subscriptions_.erase(topic); }

  std::set<std::string> get_subscriptions() const { return subscriptions_; }

protected:
  std::vector<char> receive_(const duration_type& /* timeout */, std::string& topic) override
  {
    if (topic == "") {
      topic = "TEST";
    }
    return std::vector<char>(bytesOnEachReceive, 'A');
  }

private:
  bool can_receive_;
  std::set<std::string> subscriptions_;
};

} // namespace

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<SubscriberImpl>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<SubscriberImpl>);
  BOOST_REQUIRE(!std::is_move_constructible_v<SubscriberImpl>);
  BOOST_REQUIRE(!std::is_move_assignable_v<SubscriberImpl>);
}

BOOST_AUTO_TEST_CASE(StatusChecks)
{
  SubscriberImpl theSubscriber;

  BOOST_REQUIRE(!theSubscriber.can_receive());

  theSubscriber.make_me_ready_to_receive();
  BOOST_REQUIRE(theSubscriber.can_receive());

  theSubscriber.subscribe("TEST");
  auto subs = theSubscriber.get_subscriptions();
  BOOST_REQUIRE_EQUAL(subs.size(), 1);
  BOOST_REQUIRE_EQUAL(subs.count("TEST"), 1);

  theSubscriber.unsubscribe("TEST");
  subs = theSubscriber.get_subscriptions();
  BOOST_REQUIRE_EQUAL(subs.size(), 0);

  std::string topic = "";

  BOOST_REQUIRE_NO_THROW(theSubscriber.receive(Subscriber::noblock, topic));
  BOOST_REQUIRE_NO_THROW(theSubscriber.receive(Subscriber::noblock, topic, SubscriberImpl::bytesOnEachReceive));

  BOOST_REQUIRE_EXCEPTION(theSubscriber.receive(Subscriber::noblock, topic, SubscriberImpl::bytesOnEachReceive - 1),
                          dunedaq::ipm::UnexpectedNumberOfBytes,
                          [&](dunedaq::ipm::UnexpectedNumberOfBytes) { return true; });

  theSubscriber.sabotage_my_receiving_ability();
  BOOST_REQUIRE(!theSubscriber.can_receive());

  BOOST_REQUIRE_EXCEPTION(theSubscriber.receive(Subscriber::noblock, topic),
                          dunedaq::ipm::KnownStateForbidsReceive,
                          [&](dunedaq::ipm::KnownStateForbidsReceive) { return true; });
}

BOOST_AUTO_TEST_SUITE_END()
