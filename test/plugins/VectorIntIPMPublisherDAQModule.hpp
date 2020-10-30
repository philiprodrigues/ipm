/**
 * @file VectorIntIPMPublisherDAQModule.hpp
 *
 * VectorIntIPMPublisherDAQModule is a simple DAQModule implementation that sends vectors of integers to
 * VectorIntIPMReceiverDAQModule
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef IPM_TEST_PLUGINS_VECTORINTIPMPUBLISHERDAQMODULE_HPP_
#define IPM_TEST_PLUGINS_VECTORINTIPMPUBLISHERDAQMODULE_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "ipm/Publisher.hpp"

#include "ipm/viis/Structs.hpp"

#include <ers/Issue.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace ipm {
/**
 * @brief VectorIntIPMPublisherDAQModule creates vectors of ints and sends them
 * downstream
 */
class VectorIntIPMPublisherDAQModule : public appfwk::DAQModule
{
public:
  /**
   * @brief VectorIntIPMPublisherDAQModule Constructor
   * @param name Instance name for this VectorIntIPMPublisherDAQModule instance
   */
  explicit VectorIntIPMPublisherDAQModule(const std::string& name);

  VectorIntIPMPublisherDAQModule(const VectorIntIPMPublisherDAQModule&) =
    delete; ///< VectorIntIPMPublisherDAQModule is not copy-constructible
  VectorIntIPMPublisherDAQModule& operator=(const VectorIntIPMPublisherDAQModule&) =
    delete; ///< VectorIntIPMPublisherDAQModule is not copy-assignable
  VectorIntIPMPublisherDAQModule(VectorIntIPMPublisherDAQModule&&) =
    delete; ///< VectorIntIPMPublisherDAQModule is not move-constructible
  VectorIntIPMPublisherDAQModule& operator=(VectorIntIPMPublisherDAQModule&&) =
    delete; ///< VectorIntIPMPublisherDAQModule is not move-assignable

  void init(const data_t& ) override;

private:
  // Commands
  void do_configure(const data_t& );
  void do_start(const data_t& );
  void do_stop(const data_t& );

  // Threading
  void do_work(std::atomic<bool>& running_flag);
  appfwk::ThreadHelper thread_;

  // Configuration (for validation)

  viis::Conf cfg_;
  size_t nIntsPerVector_ = 999;
  std::chrono::milliseconds queueTimeout_;
  std::unique_ptr<appfwk::DAQSource<std::vector<int>>> inputQueue_;
  std::shared_ptr<Publisher> output_;
  std::string topic_;
};

} // namespace ipm
ERS_DECLARE_ISSUE_BASE(ipm,
                       PublisherProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))
} // namespace dunedaq

#endif // IPM_TEST_PLUGINS_VECTORINTIPMPUBLISHERDAQMODULE_HPP_
