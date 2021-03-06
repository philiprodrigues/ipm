/**
 * @file Sender_test.cxx Sender class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ipm/Sender.hpp"

#define BOOST_TEST_MODULE Sender_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace dunedaq::ipm;

BOOST_AUTO_TEST_SUITE(Sender_test)

namespace {

class SenderImpl : public Sender
{

public:
  SenderImpl()
    : can_send_(false)
  {}

  void connect_for_sends(const nlohmann::json& /* connection_info */) {}
  bool can_send() const noexcept override { return can_send_; }
  void make_me_ready_to_send() { can_send_ = true; }
  void sabotage_my_sending_ability() { can_send_ = false; }

protected:
  void send_(const void* /* message */, int /* N */, const duration_type& /* timeout */, const std::string& /* metadata */) override
  {
    // Pretty unexciting stub
  }

private:
  bool can_send_;
};

} // namespace ""

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<SenderImpl>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<SenderImpl>);
  BOOST_REQUIRE(!std::is_move_constructible_v<SenderImpl>);
  BOOST_REQUIRE(!std::is_move_assignable_v<SenderImpl>);
}

BOOST_AUTO_TEST_CASE(StatusChecks)
{
  SenderImpl theSender;
  std::vector<char> random_data{ 'T', 'E', 'S', 'T' };

  BOOST_REQUIRE(!theSender.can_send());

  theSender.make_me_ready_to_send();
  BOOST_REQUIRE(theSender.can_send());

  BOOST_REQUIRE_NO_THROW(theSender.send(random_data.data(), random_data.size(), Sender::noblock));

  theSender.sabotage_my_sending_ability();
  BOOST_REQUIRE(!theSender.can_send());

  BOOST_REQUIRE_EXCEPTION(theSender.send(random_data.data(), random_data.size(), Sender::noblock),
                          dunedaq::ipm::KnownStateForbidsSend,
                          [&](dunedaq::ipm::KnownStateForbidsSend) { return true; });
}

BOOST_AUTO_TEST_CASE(BadInput)
{
  SenderImpl theSender;
  theSender.make_me_ready_to_send();

  const char* badBytes = nullptr;
  BOOST_REQUIRE_EXCEPTION(theSender.send(badBytes, 10, Sender::noblock),
                          dunedaq::ipm::NullPointerPassedToSend,
                          [&](dunedaq::ipm::NullPointerPassedToSend) { return true; });
}

BOOST_AUTO_TEST_SUITE_END()
