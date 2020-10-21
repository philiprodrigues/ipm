/**
 * @file VectorIntIPMSenderDAQModule.cxx VectorIntIPMSenderDAQModule class
 * implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "VectorIntIPMSenderDAQModule.hpp"

#include "ipm/ZmqSender.hpp"

#include "TRACE/trace.h"
#include <ers/ers.h>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "VectorIntIPMSender" // NOLINT

#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq::ipm {

VectorIntIPMSenderDAQModule::VectorIntIPMSenderDAQModule(const std::string& name)
  : appfwk::DAQModule(name)
  , thread_(std::bind(&VectorIntIPMSenderDAQModule::do_work, this, std::placeholders::_1))
  , queueTimeout_(100)
  , inputQueue_(nullptr)
{

  register_command("configure", &VectorIntIPMSenderDAQModule::do_configure);
  register_command("start", &VectorIntIPMSenderDAQModule::do_start);
  register_command("stop", &VectorIntIPMSenderDAQModule::do_stop);
}

void
VectorIntIPMSenderDAQModule::init()
{
  inputQueue_.reset(new appfwk::DAQSource<std::vector<int>>(get_config()["input"].get<std::string>()));
  std::string sender_type = get_config().value<std::string>("sender_type", "Zmq");

  if (sender_type == "Zmq") {
    output_.reset(new ZmqSender());
  }
}

void
VectorIntIPMSenderDAQModule::do_configure(const std::vector<std::string>& /*args*/)
{

  nIntsPerVector_ = get_config().value<int>("nIntsPerVector", 10);
  queueTimeout_ = std::chrono::milliseconds(get_config().value<int>("queue_timeout_ms", 100));
  output_->connect_for_sends(get_config());
}

void
VectorIntIPMSenderDAQModule::do_start(const std::vector<std::string>& /*args*/)
{
  thread_.start_working_thread();
}

void
VectorIntIPMSenderDAQModule::do_stop(const std::vector<std::string>& /*args*/)
{
  thread_.stop_working_thread();
}

void
VectorIntIPMSenderDAQModule::do_work(std::atomic<bool>& running_flag)
{
  int counter = 0;
  std::vector<int> vec;
  std::ostringstream oss;

  while (running_flag.load()) {
    if (inputQueue_->can_pop() && output_->can_send()) {

      TLOG(TLVL_TRACE) << get_name() << ": Going to receive data from inputQueue";

      try {
        inputQueue_->pop(vec, queueTimeout_);
      } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
        continue;
      }

      TLOG(TLVL_TRACE) << get_name() << ": Received vector of size " << vec.size() << " from queue, sending";
      output_->send(&vec[0], vec.size() * sizeof(int), queueTimeout_);

      counter++;
      oss << ": Sent " << counter << " vectors";
      ers::info(SenderProgressUpdate(ERS_HERE, get_name(), oss.str()));
      oss.str("");
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

} // namespace dunedaq::ipm

DEFINE_DUNE_DAQ_MODULE(dunedaq::ipm::VectorIntIPMSenderDAQModule)
