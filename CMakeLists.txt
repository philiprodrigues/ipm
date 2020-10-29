cmake_minimum_required(VERSION 3.12)
project(ipm VERSION 0.0.0)

find_package(daq-buildtools 1.2.1 REQUIRED)

daq_setup_environment()

find_package(appfwk REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(cppzmq REQUIRED)
find_package(ers REQUIRED)
find_package(nlohmann_json REQUIRED)

daq_add_library(ZmqSender.cpp ZmqReceiver.cpp ZmqPublisher.cpp ZmqSubscriber.cpp LINK_LIBRARIES appfwk::appfwk ${ZMQ})

daq_add_plugin(VectorIntIPMSenderDAQModule duneDAQModule TEST LINK_LIBRARIES ipm)
daq_add_plugin(VectorIntIPMReceiverDAQModule duneDAQModule TEST LINK_LIBRARIES ipm)

file(COPY test/singleProcess.json DESTINATION test)
file(COPY test/simpleZmqSender.json DESTINATION test)
file(COPY test/simpleZmqReceiver.json DESTINATION test)
file(COPY test/simpleZmqPublisher.json DESTINATION test)
file(COPY test/simpleZmqSubscriber.json DESTINATION test)

daq_add_unit_test(ipmSender_test   LINK_LIBRARIES ipm )
daq_add_unit_test(ipmReceiver_test LINK_LIBRARIES ipm )
daq_add_unit_test(ZmqReceiver_test LINK_LIBRARIES ipm )
daq_add_unit_test(ZmqSender_test   LINK_LIBRARIES ipm )
daq_add_unit_test(ZmqPublisher_test LINK_LIBRARIES ipm )
daq_add_unit_test(ZmqSubscriber_test   LINK_LIBRARIES ipm )


daq_install()
