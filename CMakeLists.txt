cmake_minimum_required(VERSION 3.12)
project(ipm VERSION 0.0.0)

find_package(daq-buildtools 1.2.0 REQUIRED)

daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(ers REQUIRED)
find_package(nlohmann_json REQUIRED)

daq_add_library(Direct.cpp Publisher.cpp Subscriber.cpp LINK_LIBRARIES appfwk::appfwk)

daq_add_plugin(IPMReceiver duneDAQModule TEST LINK_LIBRARIES appfwk::appfwk)
daq_add_plugin(IPMSender   duneDAQModule TEST LINK_LIBRARIES appfwk::appfwk)

daq_add_unit_test(ipmSender_test   LINK_LIBRARIES ipm )
daq_add_unit_test(ipmReceiver_test LINK_LIBRARIES ipm )
daq_add_unit_test(Direct_test      LINK_LIBRARIES ipm )
daq_add_unit_test(Publisher_test   LINK_LIBRARIES ipm )
daq_add_unit_test(Subscriber_test  LINK_LIBRARIES ipm )

daq_install()
