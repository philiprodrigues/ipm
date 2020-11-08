/**
 * @file VectorIntIPMSubscriberDAQModule.cc VectorIntIPMSubscriberDAQModule class
 * implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "VectorIntIPMSubscriberDAQModule.hpp"

#include "appfwk/cmd/Nljs.hpp"
#include "ipm/viir/Nljs.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <TRACE/trace.h>
/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "VectorIntIPMSubscriber" // NOLINT

namespace dunedaq {
namespace ipm {

VectorIntIPMSubscriberDAQModule::VectorIntIPMSubscriberDAQModule(const std::string& name)
  : appfwk::DAQModule(name)
  , thread_(std::bind(&VectorIntIPMSubscriberDAQModule::do_work, this, std::placeholders::_1))
  , outputQueue_(nullptr)
{

  register_command("configure", &VectorIntIPMSubscriberDAQModule::do_configure);
  register_command("start", &VectorIntIPMSubscriberDAQModule::do_start);
  register_command("stop", &VectorIntIPMSubscriberDAQModule::do_stop);
}

void
VectorIntIPMSubscriberDAQModule::init(const data_t& init_data)
{

  std::string Subscriber_type = "ZmqSubscriber";
  std::string topic = "VectorIntTopic";

  auto ini = init_data.get<appfwk::cmd::ModInit>();
  for (const auto& qi : ini.qinfos) {
    if (qi.name == "output") {
      ERS_INFO("VIIRDM: output queue is " << qi.inst);
      outputQueue_.reset(new appfwk::DAQSink<std::vector<int>>(qi.inst));
    }

    if (qi.name == "Subscriber_type") {
      Subscriber_type = qi.inst;
    }

    if (qi.name == "topic") {
      topic = qi.inst;
    }
  }
  
  input_ = makeIPMSubscriber(Subscriber_type);
  input_->subscribe(topic);

  // TODO: John Freeman (jcfree@fnal.gov), Oct-22-2020
  // In the next week, determine what to do if Subscriber_type isn't known
}

void
VectorIntIPMSubscriberDAQModule::do_configure(const data_t& config_data)
{
  cfg_ = config_data.get<viir::Conf>();

  nIntsPerVector_ = cfg_.nIntsPerVector;
  queueTimeout_ = static_cast<std::chrono::milliseconds>(cfg_.queue_timeout_ms);

  input_->connect_for_receives(cfg_);
}

void
VectorIntIPMSubscriberDAQModule::do_start(const data_t& /*args*/)
{
  thread_.start_working_thread();
}

void
VectorIntIPMSubscriberDAQModule::do_stop(const data_t& /*args*/)
{
  thread_.stop_working_thread();
}

void
VectorIntIPMSubscriberDAQModule::do_work(std::atomic<bool>& running_flag)
{
  size_t counter = 0;
  std::ostringstream oss;

  while (running_flag.load()) {
    if (input_->can_receive()) {

      TLOG(TLVL_TRACE) << get_name() << ": Creating output vector";
      std::vector<int> output(nIntsPerVector_);

      auto recvd = input_->receive(queueTimeout_);
      assert(recvd.data.size() == nIntsPerVector_ * sizeof(int));
      memcpy(&output[0], &recvd.data[0], sizeof(int) * nIntsPerVector_);

      oss << ": Received vector " << counter << " with size " << output.size() << " on topic " << recvd.metadata;
      ers::info(SubscriberProgressUpdate(ERS_HERE, get_name(), oss.str()));
      oss.str("");

      TLOG(TLVL_TRACE) << get_name() << ": Pushing vector into outputQueue";
      try {
        outputQueue_->push(std::move(output), queueTimeout_);
      } catch (const appfwk::QueueTimeoutExpired& ex) {
        ers::warning(ex);
      }

      TLOG(TLVL_TRACE) << get_name() << ": End of do_work loop";
      counter++;
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

} // namespace ipm
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ipm::VectorIntIPMSubscriberDAQModule)
