cmake_minimum_required(VERSION 3.12)
project(ipm VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(appfwk REQUIRED)

##############################################################################
daq_point_build_to( include )  


##############################################################################
daq_point_build_to( src )

add_library(IPM src/Direct.cpp src/Publisher.cpp src/Subscriber.cpp)
target_link_libraries(IPM appfwk::appfwk)

##############################################################################
daq_point_build_to( test )

add_library(IPMReceiver_duneDAQModule test/IPMReceiver_duneDAQModule.cpp)
target_link_libraries(IPMReceiver_duneDAQModule appfwk::appfwk)
add_library(IPMSender_duneDAQModule test/IPMSender_duneDAQModule.cpp)
target_link_libraries(IPMSender_duneDAQModule appfwk::appfwk)

##############################################################################
daq_point_build_to( unittest )

daq_add_unit_test(ipmSender_test IPM )
daq_add_unit_test(Direct_test    IPM )
daq_add_unit_test(Publisher_test    IPM )
daq_add_unit_test(Subscriber_test    IPM )

daq_install(TARGETS IPM)
