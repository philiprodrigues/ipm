/**
 * @file Publisher_test.cxx Publisher class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Publisher.hpp"

#define BOOST_TEST_MODULE Publisher_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(Publisher_test)

namespace {

class PublisherImpl : public Publisher
{

public:
  PublisherImpl()
    : can_send_(false)
  {}

  void connect_for_sends(const nlohmann::json& /* connection_info */) {}
  bool can_send() const noexcept override { return can_send_; }
  void make_me_ready_to_send() { can_send_ = true; }
  void sabotage_my_sending_ability() { can_send_ = false; }

protected:
  void publish_(const void* /* message */, int /* N */, const duration_type& /* timeout */, std::string const& /* topic */) override
  {
    // Pretty unexciting stub
  }

private:
  bool can_send_;
};

} // namespace ""

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<PublisherImpl>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<PublisherImpl>);
  BOOST_REQUIRE(!std::is_move_constructible_v<PublisherImpl>);
  BOOST_REQUIRE(!std::is_move_assignable_v<PublisherImpl>);
}

BOOST_AUTO_TEST_CASE(StatusChecks)
{
  PublisherImpl thePublisher;
  std::vector<char> random_data{ 'T', 'E', 'S', 'T' };

  BOOST_REQUIRE(!thePublisher.can_send());

  thePublisher.make_me_ready_to_send();
  BOOST_REQUIRE(thePublisher.can_send());

  BOOST_REQUIRE_NO_THROW(thePublisher.send(random_data.data(), random_data.size(), Publisher::noblock));

  thePublisher.sabotage_my_sending_ability();
  BOOST_REQUIRE(!thePublisher.can_send());

  BOOST_REQUIRE_EXCEPTION(thePublisher.send(random_data.data(), random_data.size(), Publisher::noblock),
                          dunedaq::ipm::KnownStateForbidsSend,
                          [&](dunedaq::ipm::KnownStateForbidsSend) { return true; });
}

BOOST_AUTO_TEST_CASE(BadInput)
{
  PublisherImpl thePublisher;
  thePublisher.make_me_ready_to_send();

  const char* badBytes = nullptr;
  BOOST_REQUIRE_EXCEPTION(thePublisher.send(badBytes, 10, Publisher::noblock),
                          dunedaq::ipm::NullPointerPassedToSend,
                          [&](dunedaq::ipm::NullPointerPassedToSend) { return true; });
}

BOOST_AUTO_TEST_SUITE_END()
