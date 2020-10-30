/**
 * @file VectorIntIPMPublisherDAQModule.cxx VectorIntIPMPublisherDAQModule class
 * implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "VectorIntIPMPublisherDAQModule.hpp"

#include "appfwk/cmd/Nljs.hpp"
#include "ipm/viis/Nljs.hpp"

#include "TRACE/trace.h"
#include <ers/ers.h>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "VectorIntIPMPublisher" // NOLINT

#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq::ipm {

VectorIntIPMPublisherDAQModule::VectorIntIPMPublisherDAQModule(const std::string& name)
  : appfwk::DAQModule(name)
  , thread_(std::bind(&VectorIntIPMPublisherDAQModule::do_work, this, std::placeholders::_1))
  , queueTimeout_(100)
  , inputQueue_(nullptr)
  , topic_("VectorIntTopic")
{

  register_command("configure", &VectorIntIPMPublisherDAQModule::do_configure);
  register_command("start", &VectorIntIPMPublisherDAQModule::do_start);
  register_command("stop", &VectorIntIPMPublisherDAQModule::do_stop);
}

void
VectorIntIPMPublisherDAQModule::init(const data_t& init_data)
{
  std::string publisher_type = "ZmqPublisher";

  auto ini = init_data.get<appfwk::cmd::ModInit>();
  for (const auto& qi : ini.qinfos) {
    if (qi.name == "input") {
      ERS_INFO("VIISDM: input queue is " << qi.inst);
      inputQueue_.reset(new appfwk::DAQSource<std::vector<int>>(qi.inst));
    }

    if (qi.name == "publisher_type") {
      publisher_type = qi.inst;
    }

    if (qi.name == "topic") {
      topic_ = qi.inst;
    }
  }

  output_ = makeIPMPublisher(publisher_type);

  // TODO: John Freeman (jcfree@fnal.gov), Oct-22-2020
  // In the next week, determine what to do if Publisher_type isn't known
}

void
VectorIntIPMPublisherDAQModule::do_configure(const data_t& config_data)
{
  cfg_ = config_data.get<viis::Conf>();

  nIntsPerVector_ = cfg_.nIntsPerVector;
  queueTimeout_ = static_cast<std::chrono::milliseconds>(cfg_.queue_timeout_ms);

  output_->connect_for_sends(config_data);
}

void
VectorIntIPMPublisherDAQModule::do_start(const data_t& /*args*/)
{
  thread_.start_working_thread();
}

void
VectorIntIPMPublisherDAQModule::do_stop(const data_t& /*args*/)
{
  thread_.stop_working_thread();
}

void
VectorIntIPMPublisherDAQModule::do_work(std::atomic<bool>& running_flag)
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

      TLOG(TLVL_TRACE) << get_name() << ": Received vector of size " << vec.size() << " from queue, sending to topic " << topic_;
      output_->publish(&vec[0], vec.size() * sizeof(int), queueTimeout_, topic_);

      counter++;
      oss << ": Sent " << counter << " vectors";
      ers::info(PublisherProgressUpdate(ERS_HERE, get_name(), oss.str()));
      oss.str("");
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

} // namespace dunedaq::ipm

DEFINE_DUNE_DAQ_MODULE(dunedaq::ipm::VectorIntIPMPublisherDAQModule)
