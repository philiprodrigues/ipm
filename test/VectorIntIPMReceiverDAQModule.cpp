/**
 * @file VectorIntIPMReceiverDAQModule.cc VectorIntIPMReceiverDAQModule class
 * implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "VectorIntIPMReceiverDAQModule.hpp"

#include "ipm/ZmqReceiver.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <TRACE/trace.h>
/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "VectorIntIPMReceiver" // NOLINT

namespace dunedaq {
namespace ipm {

VectorIntIPMReceiverDAQModule::VectorIntIPMReceiverDAQModule(const std::string& name)
  : appfwk::DAQModule(name)
  , thread_(std::bind(&VectorIntIPMReceiverDAQModule::do_work, this, std::placeholders::_1))
  , outputQueue_(nullptr)
  , queueTimeout_(100)
{

  register_command("configure", &VectorIntIPMReceiverDAQModule::do_configure);
  register_command("start", &VectorIntIPMReceiverDAQModule::do_start);
  register_command("stop", &VectorIntIPMReceiverDAQModule::do_stop);
}

void
VectorIntIPMReceiverDAQModule::init()
{
  outputQueue_.reset(new appfwk::DAQSink<std::vector<int>>(get_config()["output"].get<std::string>()));
  std::string receiver_type = get_config().value<std::string>("receiver_type", "Zmq");
  
  if (receiver_type == "Zmq") {
      input_.reset(new ZmqReceiver());
  }
}

void
VectorIntIPMReceiverDAQModule::do_configure(const std::vector<std::string>& /*args*/)
{
  nIntsPerVector_ = get_config().value<int>("nIntsPerVector", 10);
  queueTimeout_ = std::chrono::milliseconds(get_config().value<int>("queue_timeout_ms", 100));
  input_->connect_for_receives(get_config());
}

void
VectorIntIPMReceiverDAQModule::do_start(const std::vector<std::string>& /*args*/)
{
  thread_.start_working_thread();
}

void
VectorIntIPMReceiverDAQModule::do_stop(const std::vector<std::string>& /*args*/)
{
  thread_.stop_working_thread();
}

void
VectorIntIPMReceiverDAQModule::do_work(std::atomic<bool>& running_flag)
{
  size_t counter = 0;
  std::ostringstream oss;

  while (running_flag.load()) {
    if (input_->can_receive()) {

      TLOG(TLVL_TRACE) << get_name() << ": Creating output vector";
      std::vector<int> output(nIntsPerVector_);

      auto recvd = input_->receive(queueTimeout_);
      assert(recvd.size() == nIntsPerVector_ * sizeof(int));
      memcpy(&output[0], &recvd[0], sizeof(int) * nIntsPerVector_);

      oss << ": Received vector " << counter << " with size " << output.size();
      ers::info(ReceiverProgressUpdate(ERS_HERE, get_name(), oss.str()));
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

DEFINE_DUNE_DAQ_MODULE(dunedaq::ipm::VectorIntIPMReceiverDAQModule)
