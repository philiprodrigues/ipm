cmake_minimum_required(VERSION 3.12)
project(ipm VERSION 0.0.0)

find_package(daq-buildtools 1.2.1 REQUIRED)

daq_setup_environment()

find_package(appfwk REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(cppzmq REQUIRED)
find_package(ers REQUIRED)
find_package(nlohmann_json REQUIRED)

# We don't make a real library, but we need a target called ipm::ipm for dependents to depend on, so they can find the ipm headers
daq_add_library(dummy.cpp LINK_LIBRARIES ${ZMQ} appfwk::appfwk)

daq_add_plugin(ZmqSender duneIPM LINK_LIBRARIES ${ZMQ} appfwk::appfwk)
daq_add_plugin(ZmqReceiver duneIPM LINK_LIBRARIES ${ZMQ} appfwk::appfwk)
daq_add_plugin(ZmqPublisher duneIPM LINK_LIBRARIES ${ZMQ} appfwk::appfwk)
daq_add_plugin(ZmqSubscriber duneIPM LINK_LIBRARIES ${ZMQ} appfwk::appfwk)

daq_add_plugin(VectorIntIPMSenderDAQModule duneDAQModule TEST LINK_LIBRARIES appfwk::appfwk)
daq_add_plugin(VectorIntIPMReceiverDAQModule duneDAQModule TEST LINK_LIBRARIES appfwk::appfwk)
daq_add_plugin(VectorIntIPMSubscriberDAQModule duneDAQModule TEST LINK_LIBRARIES appfwk::appfwk)

file(COPY test/singleProcess.json DESTINATION test)
file(COPY test/simpleZmqSender.json DESTINATION test)
file(COPY test/simpleZmqReceiver.json DESTINATION test)
file(COPY test/simpleZmqPublisher.json DESTINATION test)
file(COPY test/simpleZmqSubscriber.json DESTINATION test)

daq_add_unit_test(Sender_test LINK_LIBRARIES appfwk::appfwk)
daq_add_unit_test(Receiver_test LINK_LIBRARIES appfwk::appfwk)
daq_add_unit_test(Subscriber_test LINK_LIBRARIES appfwk::appfwk)


daq_add_unit_test(ZmqSender_test LINK_LIBRARIES appfwk::appfwk)
daq_add_unit_test(ZmqReceiver_test LINK_LIBRARIES appfwk::appfwk)
daq_add_unit_test(ZmqPublisher_test LINK_LIBRARIES appfwk::appfwk)
daq_add_unit_test(ZmqSubscriber_test LINK_LIBRARIES appfwk::appfwk)
set_tests_properties(ZmqSender_test ZmqReceiver_test ZmqPublisher_test ZmqSubscriber_test PROPERTIES ENVIRONMENT "CET_PLUGIN_PATH=${CMAKE_CURRENT_BINARY_DIR}/plugins:$ENV{CET_PLUGIN_PATH}")


daq_install()
